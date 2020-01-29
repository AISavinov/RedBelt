#include "test_runner.h"
#include "profile.h"

#include <map>
#include <string>
#include <sstream>
#include <future>
#include <functional>

using namespace std;

struct Stats
{
	map<string, int> word_frequences;

	void operator += (const Stats& other)
	{
		for (auto& i : other.word_frequences)
		{
			if (word_frequences.find(i.first) == word_frequences.end())
			{
				word_frequences[i.first] = 0;
			}
			word_frequences[i.first] += i.second;
		}
	}
};

Stats ExploreLine(const set<string>& key_words, const string& line)
{
	Stats ret;
	stringstream sin(line);
	string word;
	while (sin >> word)
	{
		if (key_words.find(word) == key_words.end()) continue;
		if (ret.word_frequences.find(word) == ret.word_frequences.end()) ret.word_frequences[word] = 0;
		++ret.word_frequences[word];
	}
	return ret;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input)
{
	vector <string> lines;
	for (string line; getline(input, line);) lines.push_back(move(line));
	int threads_amount = 10;
	int n = lines.size();
	int chunk_size = n / threads_amount + n % threads_amount;

	vector <stringstream> stringstreams_data;
	for (int i = 0; i < n; i += chunk_size)
	{
		stringstreams_data.emplace_back(stringstream());
		stringstream& sout = stringstreams_data.back();
		for (int j = i; j < i + min(chunk_size, n - i); ++j)
		{
			sout << lines[j] + ' ';
		}
	}

	vector <future <Stats>> futures;
	for (auto& ss : stringstreams_data)
	{
		futures.push_back(async(ExploreLine, ref(key_words), ss.str()));
	}

	Stats result;
	for (auto& f : futures)
	{
		result += f.get();
	}

	return result;
}

void TestBasic()
{
	const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

	stringstream ss;
	ss << "this new yangle service really rocks\n";
	ss << "It sucks when yangle isn't available\n";
	ss << "10 reasons why yangle is the best IT company\n";
	ss << "yangle rocks others suck\n";
	ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

	const map<string, int> expected =
			{
					{"yangle", 6},
					{"rocks", 2},
					{"sucks", 1}
			};
//    {
//        LOG_DURATION("one thread");
//        const auto stats = ExploreKeyWordsSingleThread(key_words, ss);
//        ASSERT_EQUAL(stats.word_frequences, expected);
//    }
	{
		LOG_DURATION("multi thread");
		const auto stats = ExploreKeyWords(key_words, ss);
		ASSERT_EQUAL(stats.word_frequences, expected);
	}
}

void CustomTest()
{
	const set<string> key_words = {"one", "two", "three"};

	stringstream ss;
	ss << "this new yangle service really rocks\n";
	ss << "It sucks when yangle isn't available\n";
	ss << "10 reasons why yangle is the best IT company\n";
	ss << "yangle rocks others suck\n";
	ss << "Goondex really sucks, but yangle rocks. Use yangle\n";
	ss << "one two three";

	const map<string, int> expected =
			{
					{"one", 1},
					{"two", 1},
					{"three", 1}
			};

	const auto stats = ExploreKeyWords(key_words, ss);
	ASSERT_EQUAL(stats.word_frequences, expected);
}


int main()
{
	TestRunner tr;
	RUN_TEST(tr, TestBasic);
//    RUN_TEST(tr, CustomTest);

	return 0;
}