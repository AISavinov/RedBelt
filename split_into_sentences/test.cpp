#include "test_runner.h"

#include <vector>

using namespace std;

template <typename Token>
using Sentence = vector<Token>;

template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens)
{
	size_t size = tokens.size() - 1;
	vector<Sentence<Token>> result;
	Sentence<Token> sentence;
	bool first = false;
	for (int i = 0; i < size; i++)
	{
		if(!tokens[i].IsEndSentencePunctuation())
		{
			if (first)
			{
				result.push_back(move(sentence));
				first = false;
			}
			sentence.push_back(move(tokens[i]));
		}
		else
		{
			sentence.push_back(move(tokens[i]));
			first = true;
		}
	}
	sentence.push_back(move(tokens[size]));
	result.push_back(move(sentence));
	return result;
}


struct TestToken {
	string data;
	bool is_end_sentence_punctuation = false;
	bool IsEndSentencePunctuation() const {
		return is_end_sentence_punctuation;
	}
	bool operator==(const TestToken& other) const {
		return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
	}
};

ostream& operator<<(ostream& stream, const TestToken& token) {
	return stream << token.data;
}


void TestSplitting() {
	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"sentences"}, {"!"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into"}, {"sentences"}, {"!"}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"sentences"}, {"!", true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into"}, {"sentences"}, {"!", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"sentences"}, {"!", true}, {"!", true}, {"Without"}, {"copies"}, {".", true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into"}, {"sentences"}, {"!", true}, {"!", true}},
					                            {{"Without"}, {"copies"}, {".", true}},
			                            })
	);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSplitting);
	system("pause");
	return 0;
}