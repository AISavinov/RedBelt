#include "test_runner.h"

#include <cstdint>
#include <iterator>
#include <numeric>
#include <vector>
#include <list>
#include <array>


using namespace std;

// Это специальный тип, который поможет вам убедиться, что ваша реализация
// функции MakeJosephusPermutation не выполняет копирование объектов.
// Сейчас вы, возможно, не понимаете как он устроен, однако мы расскажем,
// почему он устроен именно так, далее в блоке про move-семантику —
// в видео «Некопируемые типы»

struct NoncopyableInt {
	int value;

	NoncopyableInt(const NoncopyableInt &) = delete;

	NoncopyableInt &operator=(const NoncopyableInt &) = delete;

	NoncopyableInt(NoncopyableInt &&) = default;

	NoncopyableInt &operator=(NoncopyableInt &&) = default;
};

template<typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {

	int size = distance(first, last);
	if (size < 0)
		return;

	list<typename RandomIt::value_type> pool;
	move(first, last, back_inserter(pool));
	size_t to_move = 0, prev_to_move = 0;

	auto it = pool.begin();
	while (!pool.empty()) {
		if (prev_to_move > to_move) {
			if (prev_to_move - to_move < to_move)
				it = prev(it, prev_to_move - to_move);
			else
				it = next(pool.begin(), to_move);
		} else if (prev_to_move < to_move) {
			if (to_move - prev_to_move < pool.size() - to_move)
				it = next(it, to_move - prev_to_move);
			else
				it = prev(pool.end(), pool.size() - to_move);
		}
		(*first++) = move(*it);
		it = pool.erase(it);
		if (pool.empty()) {
			break;
		}
		prev_to_move = to_move;
		to_move = (to_move + step_size - 1) % pool.size();
	}

}

vector<int> MakeTestVector() {
	vector<int> numbers(10);
	iota(begin(numbers), end(numbers), 0);
	return numbers;
}


void TestIntVector() {
	const vector<int> numbers = MakeTestVector();
	{
		vector<int> numbers_copy = numbers;
		MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 1);
		ASSERT_EQUAL(numbers_copy, numbers);
	}
	{
		vector<int> numbers_copy = numbers;
		MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 3);
		ASSERT_EQUAL(numbers_copy, vector<int>({0, 3, 6, 9, 4, 8, 5, 2, 7, 1}));
	}
}

bool operator==(const NoncopyableInt &lhs, const NoncopyableInt &rhs) {
	return lhs.value == rhs.value;
}

ostream &operator<<(ostream &os, const NoncopyableInt &v) {
	return os << v.value;
}

void TestAvoidsCopying() {
	vector<NoncopyableInt> numbers;
	numbers.push_back({1});
	numbers.push_back({2});
	numbers.push_back({3});
	numbers.push_back({4});
	numbers.push_back({5});

	MakeJosephusPermutation(begin(numbers), end(numbers), 2);

	vector<NoncopyableInt> expected;
	expected.push_back({1});
	expected.push_back({3});
	expected.push_back({5});
	expected.push_back({4});
	expected.push_back({2});

	ASSERT_EQUAL(numbers, expected);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestIntVector);
	RUN_TEST(tr, TestAvoidsCopying);
	return 0;
}