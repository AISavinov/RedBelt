[200~#include "test_runner.h"

#include "profile.h"

#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <random>
#include <future>
#include <mutex>
#include <cmath>

using namespace std;

#pragma once

#include <map>
#include <vector>
#include <mutex>


template<typename K, typename V>
class ConcurrentMap {
public:
	static_assert(std::is_integral_v<K>, "ConcurrentMap supports only integer keys");

	struct Access {
		~Access() {
			mutex.unlock();
		}

		std::mutex &mutex;
		V &ref_to_value;
	};

	struct Bucket {
		std::mutex mutex;
		std::map<K, V> map_;
	};

	explicit ConcurrentMap(size_t bucket_count);

	Access operator[](const K &key);

	std::map<K, V> BuildOrdinaryMap();

private:
	std::vector<Bucket> buckets_;
};


template<typename K, typename V>
ConcurrentMap<K, V>::ConcurrentMap(size_t bucket_count) {
	buckets_ = std::vector<Bucket>(bucket_count);
}


template<typename K, typename V>
typename ConcurrentMap<K, V>::Access ConcurrentMap<K, V>::operator[](const K &key) {
	const size_t i = key % buckets_.size();
	buckets_[i].mutex.lock();

	if (buckets_[i].map_.count(key) == 0) {
		buckets_[i].map_.insert({key, V()});
	}

	return {buckets_[i].mutex, buckets_[i].map_[key]};
}


template<typename K, typename V>
std::map<K, V> ConcurrentMap<K, V>::BuildOrdinaryMap() {
	std::map<K, V> result;

	for (size_t i = 0; i < buckets_.size(); ++i) {
		buckets_[i].mutex.lock();

		result.insert(buckets_[i].map_.begin(), buckets_[i].map_.end());

		buckets_[i].mutex.unlock();
	}

	return result;
}


void RunConcurrentUpdates(
		ConcurrentMap<int, int> &cm, size_t thread_count, int key_count
) {
	auto kernel = [&cm, key_count](int seed) {
		vector<int> updates(key_count);
		iota(begin(updates), end(updates), -key_count / 2);
		shuffle(begin(updates), end(updates), default_random_engine(seed));

		for (int i = 0; i < 2; ++i) {
			for (auto key : updates) {
				cm[key].ref_to_value++;
			}
		}
	};

	vector<future<void>> futures;
	for (size_t i = 0; i < thread_count; ++i) {
		futures.push_back(async(kernel, i));
	}
}

void TestConcurrentUpdate() {
	const size_t thread_count = 3;
	const size_t key_count = 50000;

	ConcurrentMap<int, int> cm(thread_count);
	RunConcurrentUpdates(cm, thread_count, key_count);

	const auto result = cm.BuildOrdinaryMap();
	ASSERT_EQUAL(result.size(), key_count);
	for (auto&[k, v] : result) {
		AssertEqual(v, 6, "Key = " + to_string(k));
	}
}

void TestReadAndWrite() {
	ConcurrentMap<size_t, string> cm(5);

	auto updater = [&cm] {
		for (size_t i = 0; i < 50000; ++i) {
			cm[i].ref_to_value += 'a';
		}
	};
	auto reader = [&cm] {
		vector<string> result(50000);
		for (size_t i = 0; i < result.size(); ++i) {
			result[i] = cm[i].ref_to_value;
		}
		return result;
	};

	auto u1 = async(updater);
	auto r1 = async(reader);
	auto u2 = async(updater);
	auto r2 = async(reader);

	u1.get();
	u2.get();

	for (auto f : {&r1, &r2}) {
		auto result = f->get();
		ASSERT(all_of(result.begin(), result.end(), [](const string &s) {
			return s.empty() || s == "a" || s == "aa";
		}));
	}
}

void TestSpeedup() {
	{
		ConcurrentMap<int, int> single_lock(1);

		LOG_DURATION("Single lock");
		RunConcurrentUpdates(single_lock, 4, 50000);
	}
	{
		ConcurrentMap<int, int> many_locks(100);

		LOG_DURATION("100 locks");
		RunConcurrentUpdates(many_locks, 4, 50000);
	}
}

int main() {
	auto size = sizeof(int);
	TestRunner tr;
	for (auto i = 0; i < 20; ++i) {
		RUN_TEST(tr, TestConcurrentUpdate);
		RUN_TEST(tr, TestReadAndWrite);
		RUN_TEST(tr, TestSpeedup);
	}

}

