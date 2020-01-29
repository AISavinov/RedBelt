#include <string>
#include <cstring>
#include <list>
#include <string_view>
#include "test_runner.h"

using namespace std;

class Editor {
public:

	void Left() {

		//if (it != text.begin())
		--it;
	}

	void Right() {
		//if (std::next(it) != text.end())
		++it;
	}

	void Insert(char token) {
		text.insert(it, token);
	}

	void Cut(size_t tokens = 1) {

		if (std::distance(it, text.end()) >= tokens) {
			buffer = list<char>(it, std::next(it, tokens));
			bufferBegin = buffer.begin();
			bufferEnd = buffer.end();
			auto newIt = std::next(it, tokens);
			text.erase(it, std::next(it, tokens));
			tempBuff = string(bufferBegin, bufferEnd);
			it = newIt;
		} else {
			buffer = list<char>(it, text.end());
			bufferBegin = buffer.begin();
			bufferEnd = buffer.end();
			auto newIt = std::next(it, tokens);
			text.erase(it, text.end());
			tempBuff = string(bufferBegin, bufferEnd);
			it = newIt;
		}
	}

	void Copy(size_t tokens = 1) {
		if (std::distance(it, text.end()) >= tokens) {
			bufferBegin = it;
			bufferEnd = std::next(it, tokens);
		} else {
			bufferBegin = it;
			bufferEnd = text.end();
		}

	}

	void Paste() {
		text.insert(it, bufferBegin, bufferEnd);
	}

	string GetText() const {
		return string(text.begin(), text.end());
	}

private:
	list<char> text;
	list<char>::iterator it = text.begin();
	list<char> buffer;
	list<char>::iterator bufferBegin;
	list<char>::iterator bufferEnd;

	string tempBuff;
};

void TypeText(Editor &editor, const string &text) {
	for (char c : text) {
		editor.Insert(c);
	}
}

void TestEditing() {
	{
		Editor editor;

		const size_t text_len = 12;
		const size_t first_part_len = 7;
		TypeText(editor, "hello,_world");
		for (size_t i = 0; i < text_len; ++i) {
			editor.Left();
		}
		editor.Cut(first_part_len);
		for (size_t i = 0; i < text_len - first_part_len; ++i) {
			editor.Right();
		}
		TypeText(editor, ",_");
		editor.Paste();
		editor.Left();
		editor.Left();
		editor.Cut(3);
//world,_hello,_
		ASSERT_EQUAL(editor.GetText(), "world,_hello");
	}
	{
		Editor editor;
//misprint
		TypeText(editor, "misprnit");
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Cut(1);
		editor.Right();
		editor.Paste();

		ASSERT_EQUAL(editor.GetText(), "misprint");
	}
}

void TestReverse() {
	Editor editor;

	const string text = "esreveR";
	for (char c : text) {
		editor.Insert(c);
		editor.Left();
	}

	ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
	Editor editor;
	ASSERT_EQUAL(editor.GetText(), "");

	editor.Left();
	editor.Left();
	editor.Right();
	editor.Right();
	editor.Copy(0);
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
	Editor editor;

	editor.Paste();
	TypeText(editor, "example");
	editor.Left();
	editor.Left();
	editor.Paste();
	editor.Right();
	editor.Paste();
	editor.Copy(0);
	editor.Paste();
	editor.Left();
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "example");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestEditing);
	RUN_TEST(tr, TestReverse);
	RUN_TEST(tr, TestNoText);
	RUN_TEST(tr, TestEmptyBuffer);
	return 0;
}