#include "pch.h"
#include <string>
#include "../TextParser/TextAnalyser.h"

#include <vector>
#include <unordered_map>
#include <map>
#include <regex>

using namespace std;
using namespace AnalyserLib;

namespace TextAnalyserTest
{
	// ---------------------------------------------------
	TEST(SplitText, SimpleWords)
	{
		stringList list = AnalyserLib::splitTextIntoStringList("Das ist ein Kleiner Text. Ganz normal ohne Zeug!");

		EXPECT_EQ(list.size(), 9);
		EXPECT_EQ(list[0], "Das");
	}
	TEST(SplitText, ALotOfSigns)
	{
		stringList list = splitTextIntoStringList("Das... ist ein    !Kleiner! Text.\nNicht-Ganz soo00        normal?");

		EXPECT_EQ(list.size(), 9);
		EXPECT_EQ(list[2], "ein");
	}
	TEST(SplitText, SplitSentence)
	{
		stringList list = splitTextIntoStringList("Das ist ein Kleiner! Text.\nNicht-Ganz soo00 normal?", false, defaultSentenceEnd);

		EXPECT_EQ(list.size(), 3);
		EXPECT_EQ(list[1], "Text");
	}
	TEST(SplitText, WithoutLastSigns)
	{
		stringList list = splitTextIntoStringList("Das ist ein");

		EXPECT_EQ(list.size(), 3);
	}

	// ---------------------------------------------------
	TEST(CountWords, CountVsSplitFunction)
	{
		string testText = "Das ist ein Kleiner Text. Ganz normal ohne Zeug!";
		stringList list = splitTextIntoStringList(testText);
		int count = countTotalWords(testText);
		EXPECT_EQ(list.size(), count);
	}

	// ---------------------------------------------------
	TEST(WordDistribution, SimpleCall)
	{
		string testText = "Das ist ein Kleiner Text. Ganz normal ohne Zeug!";
		wordDistribution distri = createWordDistribution(testText);
		for (const auto& [_, value] : distri)
			EXPECT_EQ(value, 1);
	}

	TEST(WordDistribution, SimpleCallWithCaseSensitive)
	{
		string testText = "Das zeug ist ein Kleiner Text. Ein text ohne Zeug!";
		wordDistribution distri = createWordDistribution(testText);
		for (const auto& [_, value] : distri)
			EXPECT_EQ(value, 1);
	}

	TEST(WordDistribution, SimpleCallWithoutCaseSensitive)
	{
		string testText = "Das zeug ist ein Kleiner Text. Ein text ohne Zeug!";
		wordDistribution distri = createWordDistribution(testText, true);
		EXPECT_EQ(distri["ein"], 2);
		EXPECT_EQ(distri["Ein"], 0);
		EXPECT_EQ(distri["zeug"], 2);
	}

	TEST(WordDistribution, UseStopWords)
	{
		string testText = "Das zeug ist ein Kleiner Text. Ein text ohne Zeug!";
		wordDistribution distri = createWordDistribution(testText, constStringList{ "ein", "ist" });
		EXPECT_EQ(distri["ein"], 0);
		EXPECT_EQ(distri["Ein"], 1);
		EXPECT_EQ(distri["ist"], 0);
		EXPECT_EQ(distri["zeug"], 1);
	}

	TEST(WordDistribution, UseStopWordsAndCaseSensitive)
	{
		string testText = "Das zeug ist ein Kleiner Text. Ein text ohne Zeug!";
		wordDistribution distri = createWordDistribution(testText, constStringList{ "ein", "ist" }, true);
		EXPECT_EQ(distri["ein"], 0);
		EXPECT_EQ(distri["Ein"], 0);
		EXPECT_EQ(distri["ist"], 0);
		EXPECT_EQ(distri["zeug"], 2);
	}

	TEST(WordDistribution, EmptyWordList)
	{
		wordDistribution distri = createWordDistribution(constStringList{}, constStringList{});
		EXPECT_TRUE(distri.size() == 0);
	}

	TEST(WordDistribution, UseStopWordsAndCaseSensitiveBase)
	{
		string testText = "Das zeug ist ein Kleiner Text. Ein text ohne Zeug!";
		stringList list = splitTextIntoStringList(testText, true);
		wordDistribution distri = createWordDistribution(list, constStringList{ "ein", "ist" });
		EXPECT_EQ(distri["ein"], 0);
		EXPECT_EQ(distri["Ein"], 0);
		EXPECT_EQ(distri["ist"], 0);
		EXPECT_EQ(distri["zeug"], 2);
	}
	
	// ---------------------------------------------------
	TEST(CleanUp, CleanHTML)
	{
		string testText = "<!DOCTYPE html>"
			"<html>"
			"<body>"
			"<h1>My First Heading</h1>"
			"<p>My first paragraph.</p>"
			"</body>"
			"</html>";
		cleanUpText(testText, cleanupType::HTML);
		EXPECT_TRUE(testText.find('<') == string::npos);
		EXPECT_TRUE(testText.find('>') == string::npos);
	}

	TEST(CleanUp, CleanNonSpecial)
	{
		string testText = "Hallo, welt.";
		size_t originSize = testText.size();
		cleanUpText(testText);
		EXPECT_EQ(originSize, testText.size());
	}

	TEST(CleanUp, CleanSpacesAndLinebreak)
	{
		string testText = "Hallo,   we\nlt.";
		size_t originSize = testText.size();
		cleanUpText(testText);
		EXPECT_NE(originSize, testText.size());
	}

	TEST(CleanUp, CleanAllSigns)
	{
		string testText = u8"Hallo, welt.Dies\nist\nein, robuster.Test.\n\nWörter, ohne Leerzeichen\nwerden.richtig, getrennt\nauch\nbei\nZeilenumbrüchen. ist Ist ist";
		cleanUpText(testText);
		size_t originSize = testText.size();
		cleanUpText(testText, cleanupType::removeAllSigns);
		EXPECT_NE(originSize, testText.size());
	}

	// ---------------------------------------------------
	TEST(FindPattern, FindWords)
	{
		string testText = "Das ist ein Test um worte mit vier Buchstaben zu finden.";
		auto res = findPatterns(testText, regex(R"(\b\w{4}\b)"));
		EXPECT_EQ(res.size(), 2);
		EXPECT_EQ(res[0].value, "Test");
		EXPECT_EQ(res[0].position, 12);
	}
	TEST(FindPattern, FindNoWords)
	{
		string testText = "Das ist ein Test um worte mit vier Buchstaben zu finden.";
		auto res = findPatterns(testText, regex(R"(\d+)"));
		EXPECT_EQ(res.size(), 0);
	}
	TEST(FindPattern, FindSpecificWords)
	{
		string testText = "Das ist ein Test um worte mit vier Buchstaben zu finden.";
		auto res = findPatterns(testText, regex(R"(Test)"));
		EXPECT_EQ(res.size(), 1);
		EXPECT_EQ(res[0].value, "Test");
		EXPECT_EQ(res[0].position, 12);
	}
	
	// ---------------------------------------------------
	TEST(CalculateWord, CountWordLength)
	{
		string testText = "Das ist ein mit dir gut.";
		double count = calculateWordLength(testText);
		EXPECT_EQ(count, 3);
	}

	TEST(CalculateWord, CountSentenceWordLength)
	{
		string testText = "Das ist. ein mit. dir gut.";
		double count = calculateSentenceLengthByWords(testText);
		EXPECT_EQ(count, 2);
	} 

	TEST(CalculateWord, CountSentenceLetter)
	{
		string testText = "Das ist. ein mit. dir gut.";
		double count = calculateSentenceLengthByLetters(testText);
		EXPECT_EQ(count, 6);
	}

	TEST(CalculateWord, CountWordAmount)
	{
		string testText = "Das ist ein Test weil noch einer nötig ist.";
		auto result = calculateWordLengthDistibution(testText);
		EXPECT_EQ(result[2], 0);
		EXPECT_EQ(result[3], 4);
		EXPECT_EQ(result[4], 3);
		EXPECT_EQ(result[5], 2);
	}

	// ---------------------------------------------------
	TEST(ConectedWords, SimpleFind)
	{
		string testText = "Das ist ein Test weil noch einer nötig ist, ein letzter.";
		auto result = findConnectedWords(testText,2);
		EXPECT_EQ(result.size(), 1);
		auto pair = result.begin();
		EXPECT_NE(pair, result.end());
		EXPECT_EQ(pair->first.first, "ist");
		EXPECT_EQ(pair->first.second, "ein");
		EXPECT_EQ(pair->second, 2);
	}

	// ---------------------------------------------------
	TEST(ScoreText, SimpleScore)
	{
		string testText = "Das. ist. ein. Test. weil. noch. einer. nötig. ist.";
		ReadabilityScore score = calculateGermanScore(testText);
		EXPECT_EQ(score, ReadabilityScore::VeryEasy);
	}
}