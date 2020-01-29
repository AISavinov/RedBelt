#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <vector>

using namespace std;

template<typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2)
		return;
	vector<typename RandomIt::value_type> ret(make_move_iterator(range_begin),
	                                          make_move_iterator(range_end));
	auto firstPartStart = make_move_iterator(ret.begin());
	auto firstPartEnd = make_move_iterator(ret.begin() + ret.size() / 3);
	auto secondPartEnd = make_move_iterator(ret.begin() + 2 * ret.size() / 3);
	auto thirdPartEnd = make_move_iterator(ret.end());

	MergeSort(ret.begin(),
	          ret.begin() + ret.size() / 3);
	MergeSort(ret.begin() + ret.size() / 3,
	          ret.begin() + 2 * ret.size() / 3);
	MergeSort(ret.begin() + 2 * ret.size() / 3,
	          ret.end());

	std::vector<typename RandomIt::value_type> tmp;
	std::merge(firstPartStart, firstPartEnd,
	           firstPartEnd, secondPartEnd,
	           std::back_inserter(tmp));

	std::merge(make_move_iterator(tmp.begin()), make_move_iterator(tmp.end()),
	           secondPartEnd, thirdPartEnd,
	           range_begin);
}

void TestIntVector() {
	vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
	MergeSort(begin(numbers), end(numbers));
	ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestIntVector);
	return 0;
}