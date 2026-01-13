#include "TextAnalyser.h"
//#include <iostream>
#include <locale>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <future>
#include <queue>
using namespace std;

namespace AnalyserLib
{
    /// wird benötigt damit man mit andere Zeichen als Ascii arbeiten kann
    static locale usedLocal("de_DE.UTF8");
    /// Deutsche sonderzeichen
    static const string germanSpecials = "ÄäÖöÜöß";

    /**
    * [intern]
    * Eine Helper Funktion um ein Wort in LowerCase zu convertieren
    */
    void toLowerCase(string& word)
    {
        transform(word.begin(), word.end(), word.begin(),
            [](const char& c) { return tolower(c, usedLocal); }
        );
    }

    /**
    * [intern]
    * Generiert einen sub string aus dem Text mit der übergebenen länge
    */
    inline string getTextPart(const std::string& text, const int& start, int& length, bool& useLowerCase)
    {
        string word = text.substr(start, length);
        if (useLowerCase)
            toLowerCase(word);

        length = 0;
        return word;
    }

    // Siehe Header
    stringList splitTextIntoStringList(const std::string& text, bool useLowerCase, const string& endSignes)
    {
        stringList sList;
        int length = 0, startIdx = 0;
        for (int i = 0; i < text.size(); ++i)
        {
            if (isalpha(text[i], usedLocal) || germanSpecials.find(text[i]) != string::npos)
            {
                if (length == 0)
                    startIdx = i;
                length++;
            }
            else if (length > 0)
            {
                if (endSignes.find(text[i]) != string::npos)
                    sList.push_back(getTextPart(text, startIdx, length, useLowerCase));
                else
                    length++;
            }

        }

        if (length > 0)
        {
            sList.push_back(getTextPart(text, startIdx, length, useLowerCase));
        }

        return sList;
    }

    // Siehe Header
    stringList splitTextIntoStringListAsync(const std::string& text, bool useLowerCase, const std::string& endingPattern)
    {
        future<stringList> res = async(launch::async, splitTextIntoStringList, text, useLowerCase, endingPattern);
        return res.get();
    }

    /**
    * [intern]
    * Ein struct zum speichern von statistischen Daten über einen Text
    */
    struct Stats
    {
        int letters;
        int words;
        int sentense;
        int syllable;
    };

    /// Alle gespeicherten texte in einer unordered map of Hash Stats
    static unordered_map<size_t, Stats> saveStatsManager;
    /**
    * [intern]
    * gibt eine gespeicherte Statistic zurück wenn eine passende zum Hash gefunden wird
    */
    Stats getStats(size_t hashValue)
    {
       // return { 0, 0, 0, 0 };
        auto save = saveStatsManager.find(hashValue);
        if (save == saveStatsManager.end())
            return { 0, 0, 0, 0 };
        return saveStatsManager[hashValue];
    }

    /**
    * [intern]
    * Speichert die Statistiken mit dem Hashwert des textes ab
    */
    void saveStats(size_t hashValue, Stats s)
    {
        saveStatsManager[hashValue] = s;
    }

    // Siehe Header
    void clearCache()
    {
        saveStatsManager.clear();
    }

    /**
    * [intern]
    * Erzeugt eine Statsitische erhebung von einem Text.
    */
    Stats createStatistics(const std::string& text, const string& wordEndSignes, const string& sentenceEndSignes)
    {
        size_t h1 = std::hash<std::string>{}(text);
        size_t h2 = std::hash<std::string>{}(wordEndSignes);
        size_t h3 = std::hash<std::string>{}(sentenceEndSignes);
        size_t hashValue = h1 ^ h2 ^ h3;
        
        Stats stat = getStats(hashValue);
        if (stat.letters > 0)
            return stat;

        bool isWordStart = false;
        bool isLastOneVowel = false;
        static const string syllable(syllableDetection);

        for (int i = 0; i < text.size(); ++i)
        {
            if (isalpha(text[i], usedLocal) || germanSpecials.find(text[i]) != string::npos)
            {
                stat.letters++;
                bool isVowel = syllable.find(text[i]) != string::npos;
                if (isVowel && !isLastOneVowel)
                    stat.syllable++;
                isLastOneVowel = isVowel;
                isWordStart = true;
            }
            else if (isWordStart && wordEndSignes.find(text[i]) != string::npos)
            {
                stat.words++;
                if (sentenceEndSignes.find(text[i]) != string::npos)
                    stat.sentense++;
                isWordStart = false;
            }
        }

        if (isWordStart)
        {
            stat.words++;
            stat.sentense++;
        }

        saveStats(hashValue, stat);
        return stat;
    }

    // Siehe Header
    int countTotalWords(const std::string& text)
    {
        auto stat = createStatistics(text, defaultWordEnd, defaultSentenceEnd);

        return stat.words;
    }

    // Siehe Header
    int countTotalWordsAsync(const std::string& text)
    {
        future<int> res = async(launch::async, countTotalWords, text);
        return res.get();
    }

    // Siehe Header
    wordDistribution createWordDistribution(const std::string& text, bool caseSensitive)
    {
        return createWordDistribution(text, stringList(), caseSensitive);
    }

    // Siehe Header
    wordDistribution createWordDistribution(const std::string& text, constStringList& stopWords, bool caseSensitive)
    {
        auto words = splitTextIntoStringList(text, caseSensitive);
        return createWordDistribution(words, stopWords);
    }

    // Siehe Header
    wordDistribution createWordDistribution(constStringList& wordList, constStringList& stopWords)
    {
        unordered_map<string, int> wordDistibution;
        for (const string& w : wordList)
        {
            // check for stop words
            auto itStopwords = find(stopWords.begin(), stopWords.end(), w);
            if (itStopwords == stopWords.end())
                wordDistibution[w]++;
        }
        return wordDistibution;
    }

    /**
    * [intern]
    * helper function weil future und async kein überladenen functionen unterstützt
    */
    wordDistribution createWordDistributionAsyncHelper(constStringList& wordList, constStringList& stopWords)
    {
        return createWordDistribution(wordList, stopWords);
    }
    // Siehe Header
    wordDistribution createWordDistributionAsync(constStringList& wordList, constStringList& stopWords)
    {
        future<wordDistribution> res = async(launch::async, createWordDistributionAsyncHelper, wordList, stopWords);
        return res.get();
    }

    /**
    * [intern]
    * Entfernt nur basis HTML tags und ist nicht für Komplexe HTML Dateien gedacht.
    * An dieser Stelle ist es zu empfehlen einen Externen Parser einzufügen
    */
    void cleanUpHtml(string& text)
    {
        size_t startIdx = text.find('<'), endIdx = 0;

        while(startIdx != string::npos)
        {
            endIdx = text.find('>');
            if (endIdx == string::npos)
            {
                throw (string)"Html Text hat nicht das richtige Format";
                break;
            }

            // create spaces between tags
            text[endIdx] = ' ';
            text.erase(startIdx, endIdx - startIdx);
            
            startIdx = text.find('<');
        }
    }

    /**
    * [intern]
    * ersetzt aus eine Liste von Zeichen jedes Zeichen durch ein Leerzeichen
    */
    void cleanUpSigns(std::string& text, const string& removeSigns)
    {
        for (const char& c : removeSigns)
            replace(text.begin(), text.end(), c, ' ');
    }

    /**
    * [intern]
    * Entfernt nur ein Teil vom Markdown tags und man muss definitv hier einen Parser einbauen der alles handlen kann.
    */
    void cleanUpMarkdown(string& text)
    {
        cleanUpSigns(text, "*~_#");
        searchList bolds = findPatterns(text, regex(R"(bold)"));
        for(auto b : bolds)
            text.erase(b.position, 4);
    }

    /**
    * [intern]
    * Entfernt überflüssige leerzeichen
    */
    void cleanUpSpaces(string& text)
    {
        auto newEnd =
            unique(text.begin(), text.end(),
                [=](char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }
            );
        text.erase(newEnd, text.end());

    }
    
    // Siehe Header
    void cleanUpText(std::string& text, cleanupType type)
    {
        if (type == None)
            return;

        replace(text.begin(), text.end(), '\n', ' ');
        switch (type)
        {
            case cleanupType::removeAllSigns: cleanUpSigns(text, ",.!?-"); break;
            case cleanupType::HTML: cleanUpHtml(text); break;
            case cleanupType::Markdown: cleanUpMarkdown(text); break;
        }
        cleanUpSpaces(text);
    }

    // Siehe Header
    std::string readFile(const std::string& path, cleanupType type)
    {
        ifstream file(path);
        if (file.is_open())
        {
            string fileText, line;
            while(getline(file,line))
                fileText += line;

            if (!file.eof())
                fileText = "Err";

            file.close();

            cleanUpText(fileText, type);
            return fileText;
        }
        return "";
    }

    // Siehe Header
    searchList findPatterns(const std::string& text, const std::regex& pattern)
    {
        searchList occurVec;
        const vector<smatch> matches{
            sregex_iterator{text.begin(), text.end(), pattern},
            sregex_iterator{}
        };

        for (auto i : matches)
        {
            int pos = i[0].first - text.begin();
            occurVec.push_back({ i.str(), pos });
        }

        return occurVec;
    }

    // Siehe Header
    double calculateWordLength(const std::string& text)
    {
        auto stat = createStatistics(text, defaultWordEnd, defaultSentenceEnd);
        return static_cast<double>(stat.letters) / stat.words;
    }

    // Siehe Header
    double calculateSentenceLengthByWords(const std::string& text)
    {
        auto stat = createStatistics(text, defaultWordEnd, defaultSentenceEnd);
        return static_cast<double>(stat.words) / stat.sentense;
    }

    // Siehe Header
    double calculateSentenceLengthByLetters(const std::string& text)
    {
        auto stat = createStatistics(text, defaultWordEnd, defaultSentenceEnd);
        return static_cast<double>(stat.letters) / stat.sentense;
    }

    // Siehe Header
    wordLengthDistibution calculateWordLengthDistibution(const std::string& text, const std::string& wordEndSignes)
    {
        wordLengthDistibution stat;
        int wordLength = 0;

        for (int i = 0; i < text.size(); ++i)
        {
            if (isalpha(text[i], usedLocal) || germanSpecials.find(text[i]) != string::npos)
            {
                wordLength++;
            }
            else if (wordLength > 0 && wordEndSignes.find(text[i]) != string::npos)
            {
                stat[wordLength]++;
                wordLength = 0;
            }
        }

        if (wordLength > 0)
        {
            stat[wordLength]++;
            wordLength = 0;
        }
        return stat;
    }

    // Siehe Header
    wordLengthDistibution calculateWordLengthDistibutionAsync(const std::string& text, const std::string& wordEndSignes)
    {
        future<wordLengthDistibution> res = async(launch::async, calculateWordLengthDistibution, text, wordEndSignes);
        return res.get();
    }

    // Siehe Header
    wordPairs findConnectedWords(const std::string& text, int minOccurrences, int range)
    {
        unordered_map<strPair, int, PairHash> collection;

        if (range < 2 || minOccurrences < 1)
            return collection;

        stringList sentenceList = splitTextIntoStringList(text, true, defaultSentenceEnd);
        for (const string& sentence : sentenceList)
        {
            stringList wordList = splitTextIntoStringList(sentence, true, defaultWordEnd);
            for (int i = 1; i < min(range, static_cast<int>(wordList.size())); ++i)
            {
                for (auto it = wordList.begin(); it != wordList.end() - i; it++)
                    collection[{*it, * (it + i)}]++;
            }
        }
        for (auto it = collection.begin(); it != collection.end();)
        {
            if (it->second < minOccurrences)
                it = collection.erase(it);
            else
                it++;
        }

        return collection;
    }

    // Siehe Header
    wordPairs findConnectedWordsAsync(const std::string& text, int minOccurrences, int range)
    {
        future<wordPairs> res = async(launch::async, findConnectedWords, text, minOccurrences, range);
        return res.get();
    }

    // Siehe Header
    ReadabilityScore calculateGermanScore(const std::string& text)
    {
        auto stat = createStatistics(text, defaultWordEnd, defaultSentenceEnd);
        double score = 180 - (static_cast<double>(stat.words) / stat.sentense) - 58.5 * (static_cast<double>(stat.syllable) / stat.words);
        
        if (score >= 90)    //    90 - 100: Sehr leicht.Geeignet für einen 11 jährigen Schüler.
            return VeryEasy;
        
        if (score >= 80)    //    80 - 90 : Leicht
            return Easy;
        
        if (score >= 70)    //    70 - 80 : Mittelleicht
            return MediumEasy;
        
        if (score >= 60)    //    60 - 70 : Mittel.Geeignet für 13 - 15 jährige Schüler.
            return Medium;
        
        if (score >= 50)    //    50 - 60 : Mittelschwer
            return MediumHard;
        
        if (score >= 30)    //    30 - 50 : Schwer
            return Hard;
        
        return VeryHard;    //    0 - 30 : Sehr schwer.Geeignet für Akademiker.
    }
}
