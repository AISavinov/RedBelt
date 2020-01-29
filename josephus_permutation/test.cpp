#include "test_runner.h"

#include <cstdint>
#include <iterator>
#include <numeric>
#include <vector>

using namespace std;

template<typename RandomIt>
void printRange(RandomIt first, RandomIt last) {
	for (auto it = first; it != last; ++it) {
		cout << *it << ", ";
	}
	cout << endl;
}

template<typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
	vector<typename RandomIt::value_type> pool(first, last);
	size_t cur_pos = 0;
	while (!pool.empty()) {
		*(first++) = pool[cur_pos];
		pool.erase(pool.begin() + cur_pos);
		if (pool.empty()) {
			break;
		}
		cur_pos = (cur_pos + step_size - 1) % pool.size();
		cout<< cur_pos;
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
		//vector<int> numbers_copy = numbers;
		//MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 1);
		//ASSERT_EQUAL(numbers_copy, numbers);
	}
	{
		vector<int> numbers_copy = numbers;
		MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 3);
		ASSERT_EQUAL(numbers_copy, vector<int>({0, 3, 6, 9, 4, 8, 5, 2, 7, 1}));
	}
}


struct NoncopyableInt {
	int value;
};

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
	//RUN_TEST(tr, TestAvoidsCopying);
	return 0;
}