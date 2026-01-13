#include <iostream>
#include <string>
#include "TextAnalyser.h"
#include <windows.h>
#include <chrono>

using namespace std;
using namespace AnalyserLib;

// Beispielprogramme 1: Zähle Wortverteilung
void NimmaMehr(const string& path)
{
    string nimmaText = readFile(path);
    auto list = splitTextIntoStringList(nimmaText, true);

    auto words = createWordDistribution(list, stringList{"der","die","das", "ist", "und", "ich"});
    int bestValue = 0;
    for (auto w : words)
    {
        if (w.second > bestValue)
            bestValue = w.second;
    }
    cout << "Im 'Rabe' von Edgar Alan Poe kommen folgende Worte:\n";
    for (auto w : words)
    {
        if (w.second == bestValue)
            cout << w.first << endl;
    }
    cout << bestValue << " mal vor \n";
}

// Beispielprogramme 2: Berechne wort verteilung und länge
void Faust(const string& path)
{
    string faustText = readFile(path);
    
    cout << "Faust von Goethe wird ausgewertet:\n";
    cout << countTotalWords(faustText) << " Wörter" << endl;
    cout << "Durchschnittliche Wortlänge " << calculateWordLength(faustText) << endl;
    cout << "Durchschnittliche Wörter pro Satz " << calculateSentenceLengthByWords(faustText) << endl;

    cout << "Score : " << scoreToString(calculateGermanScore(faustText)) << endl;
}

// Beispielprogramme 3: Finde Patterns
void DoBaseTest()
{
    string langerText = "In der heutigen digitalen Welt spielt Sprache eine zentrale Rolle. Menschen kommunizieren täglich über Nachrichten, soziale Medien und E-Mails. Dabei entstehen große Mengen an Text, die analysiert werden können. Die Analyse von Texten hilft dabei, Muster zu erkennen und Informationen zu gewinnen. Ein häufig untersuchtes Merkmal ist die Wortlänge. Kurze Wörter treten oft in alltäglichen Gesprächen auf, während längere Wörter häufiger in wissenschaftlichen Texten vorkommen. Neben der Wortlänge ist auch die Häufigkeit einzelner Wörter von Interesse. Bestimmte Begriffe tauchen immer wieder auf und prägen den Inhalt eines Textes."
        "Ein weiterer wichtiger Ansatz ist die Ko - Okkurrenz - Analyse.Sie untersucht, welche Wörter häufig gemeinsam auftreten.So lassen sich thematische Zusammenhänge erkennen.Wenn Wörter wie Analyse, Daten und Statistik oft zusammen vorkommen, deutet das auf einen fachlichen Kontext hin.In literarischen Texten hingegen erscheinen andere Wortkombinationen.Die Kombination verschiedener Methoden ermöglicht eine tiefere Einsicht in den Aufbau und die Bedeutung von Texten.Textanalyse ist deshalb ein wichtiges Werkzeug in der Informatik, der Linguistik und der künstlichen Intelligenz.Sie bildet die Grundlage für Suchmaschinen, automatische Übersetzunge und viele moderne Anwendungen.";

    auto con = findConnectedWords(langerText, 2, 4);
    for (auto c : con)
        cout << c.first.first << "-" << c.first.second << " = " << c.second << endl;
}

void BenchMarkRun(const string& originalText, const string& name, void(*f)(string&))
{
    string testText = originalText;
    int testCycles = 5;
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < testCycles; ++i)
    {
        testText = originalText;
        f(testText);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << name << " : Zeit für " << testCycles << " Durchläufe " << duration.count() << endl;
}

void BenchMark()
{
    string originalText = readFile("Goethe--Faust.txt");
    BenchMarkRun(originalText, "splitTextIntoStringList(text)", [](string& str) {splitTextIntoStringList(str); });
    BenchMarkRun(originalText, "splitTextIntoStringList(text, true)", [](string& str) {splitTextIntoStringList(str, true); });

    BenchMarkRun(originalText, "countTotalWords(text)", [](string& str) {countTotalWords(str); });

    BenchMarkRun(originalText, "createWordDistribution(text)", [](string& str) {createWordDistribution(str); });
    BenchMarkRun(originalText, "createWordDistribution(text, true)", [](string& str) {createWordDistribution(str, true); });

    BenchMarkRun(originalText, "cleanUpText(text)", [](string& str) {cleanUpText(str); });
    BenchMarkRun(originalText, "cleanUpText(text,cleanupType::removeAllSigns)", [](string& str) {cleanUpText(str,cleanupType::removeAllSigns); });

    BenchMarkRun(originalText, "findPatterns(text, \\b\\w{4}\\b)", [](string& str) {findPatterns(str, regex(R"(\b\w{6}\b)")); });

    BenchMarkRun(originalText, "calculateWordLength(text)", [](string& str) {calculateWordLength(str); });
    BenchMarkRun(originalText, "calculateSentenceLengthByWords(text)", [](string& str) {calculateSentenceLengthByWords(str); });
    BenchMarkRun(originalText, "calculateSentenceLengthByLetters(text)", [](string& str) {calculateSentenceLengthByLetters(str); });

    BenchMarkRun(originalText, "calculateWordLengthDistibution(text)", [](string& str) {calculateWordLengthDistibution(str); });

    BenchMarkRun(originalText, "findConnectedWords(text)", [](string& str) {findConnectedWords(str); });
    BenchMarkRun(originalText, "findConnectedWords(text, 2, 4)", [](string& str) {findConnectedWords(str, 2, 4); });
    BenchMarkRun(originalText, "findConnectedWords(text, 2, 99)", [](string& str) {findConnectedWords(str, 2, 99); });
	
    BenchMarkRun(originalText, "calculateGermanScore(text)", [](string& str) {calculateGermanScore(str); });
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    //DoBaseTest();
    //NimmaMehr("Rabe.txt");
    //Faust("Goethe--Faust.txt");
    BenchMark();
}
