#include "test_runner.h"

#include <vector>
#include <algorithm>
using namespace std;

template<typename Token>
using Sentence = vector<Token>;

template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens)
{
	vector<Sentence<Token>> result;

	auto sentence_start_it = tokens.begin();
	auto if_end_token_lambda = [](const Token &token) {
		return token.IsEndSentencePunctuation();
	};
	while (sentence_start_it != tokens.end())
	{
		// End sentence tokens begin    (for !!!! in the end)
		auto sentence_end_it_begin = find_if(sentence_start_it, tokens.end(),
		                                     if_end_token_lambda);

		auto sentence_end_it = find_if_not(sentence_end_it_begin, tokens.end(),
		                                   if_end_token_lambda);

		// #1 Вариант в два мува, быстрее чем #2
		// ~ 1000 ms
		vector<Token> tmp;
		tmp.reserve(sentence_end_it - sentence_start_it);
		move(sentence_start_it, sentence_end_it, back_inserter(tmp));
		result.push_back(move(tmp));

		// #2
		// ~ 1500 ms
		// result.push_back({});
		// result.reserve(sentence_end_it - sentence_start_it);
		// move(sentence_start_it, sentence_end_it,  back_inserter(result.back()));

		sentence_start_it = sentence_end_it;
	}
	return result;
}


struct TestToken {
	string data;
	bool is_end_sentence_punctuation = false;

	bool IsEndSentencePunctuation() const {
		return is_end_sentence_punctuation;
	}

	bool operator==(const TestToken &other) const {
		return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
	}
};

ostream &operator<<(ostream &stream, const TestToken &token) {
	return stream << token.data;
}


void TestSplitting() {

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split",     true},
			                                      {"into",      true},
			                                      {"sentences", true},
			                                      {"!"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}, {"into", true},
							                            {"sentences", true}},
					                            {{"!"}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"},
			                                      {"into"},
			                                      {"sentences"},
			                                      {"!"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into"}, {"sentences"}, {"!"}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"},
			                                      {"into"},
			                                      {"sentences"},
			                                      {"!", true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into"}, {"sentences"}, {"!", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split", true},
			                                      {"into",  true},
			                                      {"sentences"},
			                                      {"!",     true},
			                                      {"!"},
			                                      {"!",     true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}, {"into", true}},
					                            {{"sentences"},   {"!",    true}, {"!"}, {"!", true}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split", true},
			                                      {"into",  true},
			                                      {"sentences"},
			                                      {"kek",   true},
			                                      {"!",     true},
			                                      {"!"},
			                                      {"!",     true},
			                                      {"!",     true}}
			)),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}, {"into", true}},
					                            {{"sentences"},   {"kek",  true}, {"!", true}},
					                            {{"!"},           {"!",    true}, {"!", true}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split", true},
			                                      {"into",  true},
			                                      {"sentences"},
			                                      {"!"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}, {"into", true}},
					                            {{"sentences"},   {"!"}}
			                            })
	);
	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split",     true},
			                                      {"into",      true},
			                                      {"sentences", true},
			                                      {"!",         true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}, {"into", true},
							                            {"sentences", true}, {"!", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"},
			                                      {"into"},
			                                      {"sentences"},
			                                      {"!"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into"},
							                            {"sentences"}, {"!"}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split",     true},
			                                      {"into"},
			                                      {"sentences", true},
			                                      {"!"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}},
					                            {{"into"},
							                            {"sentences", true}, {"!"}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split", true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"},
			                                      {"into", true},
			                                      {"sentences"},
			                                      {"!",    true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into", true},
							                            {"sentences"}, {"!", true}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"},
			                                      {"into",      true},
			                                      {"sentences", true},
			                                      {"!",         true},
			                                      {"end"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}, {"into", true},
							                            {"sentences", true}, {"!", true}},
					                            {{"end"}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split", true},
			                                      {"into",  true},
			                                      {"sentences"},
			                                      {"end"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}, {"into", true}},
					                            {{"sentences"},   {"end"}}
			                            })
	);


	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split"}})),
			vector<Sentence<TestToken>>({
					                            {{"Split"}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Split",     true},
			                                      {"into"},
			                                      {"sentences", true},
			                                      {"!"},
			                                      {"end",       true}})),
			vector<Sentence<TestToken>>({
					                            {{"Split", true}},
					                            {{"into"},
							                            {"sentences", true}, {"!"}, {"end", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Start"},
			                                      {"Split",     true},
			                                      {"into"},
			                                      {"sentences", true},
			                                      {"!"},
			                                      {"end",       true},
			                                      {"asd"}})),
			vector<Sentence<TestToken>>({
					                            {{"Start"}, {"Split", true}, {"into"},
							                            {"sentences", true}, {"!"}, {"end", true}, {"asd"}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"into",      true},
			                                      {"sentences", true},
			                                      {"!",         true},
			                                      {"end"}})),
			vector<Sentence<TestToken>>({
					                            {{"into", true},
							                            {"sentences", true}, {"!", true}},
					                            {{"end"}}
			                            })
	);
	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"into",      true},
			                                      {"sentences", true},
			                                      {"!",         true}
			                                     })),
			vector<Sentence<TestToken>>({
					                            {{"into", true},
							                            {"sentences", true}, {"!", true}}
			                            })
	);
	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"into",      true},
			                                      {"sentences", true}
			                                     })),
			vector<Sentence<TestToken>>({
					                            {{"into", true},
							                            {"sentences", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"into", true}
			                                     })),
			vector<Sentence<TestToken>>({
					                            {{"into", true}}
			                            })
	);

	ASSERT_EQUAL(
			SplitIntoSentences(vector<TestToken>({{"Start"},
			                                      {"Split",     true},
			                                      {"sentences", true},
			                                      {"!"},
			                                      {"!"},
			                                      {"!"},
			                                      {"end",       true},
			                                      {"asd"}})),
			vector<Sentence<TestToken>>({
					                            {{"Start"}, {"Split", true},
							                                       {"sentences", true}},
					                            {{"!"},     {"!"}, {"!"}, {"end", true}, {"asd"}}
			                            })
	);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSplitting);
	return 0;
}