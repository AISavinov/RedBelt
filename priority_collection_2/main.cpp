#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template<typename T>
class PriorityCollection {
public:
	using Id = int;
	using Score = int;

	// Добавить объект с нулевым приоритетом
	// с помощью перемещения и вернуть его идентификатор
	Id Add(T object) {
		data.push_back(move(object));
		auto[iter, flag] = scores.insert({0, data.size() - 1});
		ids.push_back(iter);
		return data.size() - 1;
	}

	// Добавить все элементы диапазона [range_begin, range_end)
	// с помощью перемещения, записав выданные им идентификаторы
	// в диапазон [ids_begin, ...)
	template<typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end,
	         IdOutputIt ids_begin) {
		for (auto it = range_begin; it != range_end; it++) {
			*ids_begin++ = Add(move(*it));
		}
	}

	// Определить, принадлежит ли идентификатор какому-либо
	// хранящемуся в контейнере объекту
	bool IsValid(Id id) const {
		return ids[id] != scores.end();
	}

	// Получить объект по идентификатору
	const T &Get(Id id) const {
		return data[id];
	}

	// Увеличить приоритет объекта на 1
	void Promote(Id id) {
		auto[prior_score, _] =scores.extract(ids[id]).value();
		auto[new_iter, flag] = scores.insert({++prior_score, id});
		ids[id] = new_iter;
	}

	// Получить объект с максимальным приоритетом и его приоритет
	pair<const T &, int> GetMax() const {
		auto it = *prev(scores.end());
		return {Get(it.second), it.first};
	}

	// Аналогично GetMax, но удаляет элемент из контейнера
	pair<T, int> PopMax() {
		auto it = prev(scores.end());
		auto[score, max_id] = *it;
		T ret_obj = move(*(data.begin() + max_id));
		//ids.erase(ids.begin() + max_id);
		scores.erase(it);
		ids[max_id] = scores.end();
		return {move(ret_obj), score};
	}

private:

	set<pair<Score, Id>> scores;
	vector<T> data;
	vector<typename set<pair<Score, Id>>::iterator> ids;
};


class StringNonCopyable : public string {
public:
	using string::string;  // Позволяет использовать конструкторы строки
	StringNonCopyable(const StringNonCopyable &) = delete;

	StringNonCopyable(StringNonCopyable &&) = default;

	StringNonCopyable &operator=(const StringNonCopyable &) = delete;

	StringNonCopyable &operator=(StringNonCopyable &&) = default;


};

void TestNoCopy() {
	PriorityCollection<StringNonCopyable> strings;
	const auto white_id = strings.Add("white");
	const auto yellow_id = strings.Add("yellow");
	const auto red_id = strings.Add("red");
	strings.Promote(white_id);
	strings.Promote(red_id);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(red_id);
		strings.Promote(white_id);
		strings.Promote(yellow_id);//2
	}

	PriorityCollection<string> test;
	vector<string> tmp = {"1", "2"};
	vector<int> ids;
	test.Add(tmp.begin(), tmp.end(), back_inserter(ids));

	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(strings.IsValid(red_id), false);
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 3);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 3);
	}
	{
		const auto item = strings.GetMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	const auto item = strings.GetMax();
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestNoCopy);
	return 0;
}
