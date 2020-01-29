#include "test_runner.h"
#include <vector>
#include <future>
#include <algorithm>
#include <numeric>

using namespace std;

template<typename T>
int64_t CalculateSubMatrixSum(T beginIt, T endIt) {
	int64_t res = 0;
	for (auto rowIt = beginIt; rowIt != endIt; ++rowIt) {
		res += accumulate((*rowIt).begin(), (*rowIt).end(), 0);
	}
	return res;
}

int64_t CalculateMatrixSum(const vector<vector<int>> &matrix) {
	int64_t res = 0;
	vector<future<int64_t>> futures;
	size_t page_size = 2500;
	auto itBegin = matrix.begin();
	auto itEnd = page_size < matrix.end() - itBegin ? itBegin + page_size : matrix.end();
	while (itBegin != matrix.end()) {
		futures.push_back(async([itBegin, itEnd] { return CalculateSubMatrixSum(itBegin, itEnd); }));
		itBegin = itEnd;
		itEnd = page_size < matrix.end() - itEnd ? itEnd + page_size : matrix.end();
	}
	for (auto &f : futures)
		res += f.get();
	return res;
}

void TestCalculateMatrixSum() {
	const vector<vector<int>> matrix = {
			{1,  2,  3,  4},
			{5,  6,  7,  8},
			{9,  10, 11, 12},
			{13, 14, 15, 16}
	};
	ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestCalculateMatrixSum);
}