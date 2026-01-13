/**
* TextAnalyser.h
* 
* Das ist eine Bibliothek für die Analyse von Texten. Texte können aus Dateien geladen und bereinigt werden.
* Sie enthält Basisfunktionen wie Wortanzahl, Wortverteilung, Wort- oder Satzlänge.
* Zusätzlich kann man sich auch den Lesbarkeitswert berechnen lassen oder häufige vorkommen von wortpaaren ausgeben lassen
* 
* Autor: Alexander Stiller
* Version: 1.0
*/
#pragma once
#ifndef TEXTANALYSER
#define TEXTANALYSER

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <regex>

namespace AnalyserLib
{
	/// defines für bessere lesbarkeit
	using wordDistribution = std::unordered_map<std::string, int>;
	using constStringList = const std::vector<std::string>;
	using wordLengthDistibution = std::map<int, int>;

	#define defaultWordEnd " .,-!?\n"
	#define defaultSentenceEnd ".!?"
	#define syllableDetection "aeiouy"

	/// ein vector of string, bessere lesbarkeit
	using stringList = std::vector<std::string>;
	/// ein const vector of string, bessere lesbarkeit
	using constStringList = const std::vector<std::string>;

	/**
	* Unterteilt einen Text in kleinere Segmente. Je nachdem was für Zeichen übergeben werde kann man in Worte, Sätze oder was gewünscht unterteilen.
	*
	* @param	text: der übergebene Text der unterteilt werden soll
	*			useLowerCase (default = false): wenn es gesetzt ist wird der text in lowercase übertragen
	*			endingPattern (defult = defaultWordEnd " .,-!?\n"): Jedes Zeichen in dem string sogt dafür das ein Teilstring erzeugt wird.
	* @return	Gibt ein vector of String mit den unterteilten strings zurück
	*/
	stringList splitTextIntoStringList(const std::string& text, bool useLowerCase = false, const std::string& endingPattern = defaultWordEnd);

	/**
	* Async version
	*/
	stringList splitTextIntoStringListAsync(const std::string& text, bool useLowerCase = false, const std::string& endingPattern = defaultWordEnd);

	/**
	* Zählt alle Worte in dem übergebenen Text
	*
	* @param	text: ein Text im dem alle Wörter gezählt werden sollen
	* @return	Die Anzahl aller Worte
	*/
	int countTotalWords(const std::string& text);

	/**
	* Async version
	*/
	int countTotalWordsAsync(const std::string& text);

	/**
	* Zählt die Häufigkeit jedes Wortes im Text. Convertiert intern den Text in eine Liste von Wörtern
	*
	* @param	text: Der Text in dem alle Wörter aufgelistet werden sollen
	*			useLowerCase (default = false): wenn useLowerCase gesetzt ist wird der jedes wort in lowercase convertiert
	* @return	Gibt eine Unordered map of string int zurück. Key ist das Wort und der Value ist die Häufigkeit wie oft ein Wort vorgekommen ist
	*/
	wordDistribution createWordDistribution(const std::string& text, bool useLowerCase = false);
	
	/**
	* Zählt die Häufigkeit jedes Wortes im Text. Convertiert intern den Text in eine Liste von Wörtern
	*
	* @param	text: Der Text in dem alle Wörter aufgelistet werden sollen
	*			stopWords: Eine Liste von Strings die ignoriert werden sollen und somit nicht mitgezählt werden
	*			useLowerCase (default = false): wenn useLowerCase gesetzt ist wird der jedes wort in lowercase convertiert
	* @return	Gibt eine Unordered map of string int zurück. Key ist das Wort und der Value ist die Häufigkeit wie oft ein Wort vorgekommen ist
	*/
	wordDistribution createWordDistribution(const std::string& text, constStringList& stopWords, bool caseSensitive = false);
	
	/**
	* Zählt die Häufigkeit jedes Wortes im Text
	*
	* @param	wordList: Eine Liste von Wörtern die evaluiert werden soll
	*			stopWords: Eine Liste von Strings die ignoriert werden sollen und somit nicht mitgezählt werden
	* @return	Gibt eine Unordered map of string int zurück. Key ist das Wort und der Value ist die Häufigkeit wie oft ein Wort vorgekommen ist
	*/
	wordDistribution createWordDistribution(constStringList& wordList, constStringList& stopWords);

	/**
	* Async version
	*/
	wordDistribution createWordDistributionAsync(constStringList& wordList, constStringList& stopWords);

	/**
	* Enum zum einstellen mit welcher methode der Text aufgeräumt werden soll
	*/
	enum cleanupType
	{
		None = 0,
		plain = 1,
		removeAllSigns = 2,
		HTML = 4,
		Markdown = 8
	};
	/**
	* Entfernt unerwünschte zeichen aus dem Text. Zeilenumbrüche und überflüssige Leerzeichen werden immer entfernt
	*
	* @param	origin: Der Text der aufgeräumt werden soll. Origin wird in dieser Funktion direkt angepasst und ist gleichzeitig der return Wert.
	*			type (default=plain): Definiert die Methode mit der der Text aufgeräumt werden soll
	* @return	void
	*/
	void cleanUpText(std::string& origin, cleanupType type = cleanupType::plain);

	/**
	* Läd eine text datei und gibt einen aufgeräumten String zurück
	*
	* @param	path: Der Pfad und die Datei die geladen werden soll
	*			type (default=plain): Definiert die Methode mit der der Text aufgeräumt werden soll
	* @return	Gibt den geladenen und aufgeräumten Text zurück. Bei Fehlern einen leeren string.
	*/
	std::string readFile(const std::string& path, cleanupType type = cleanupType::plain);

	/**
	* Speichert den string und die Position aus der Suche nach bestimmten Mustern
	*/
	struct SearchResult
	{
		std::string value;
		int position;
	};
	/// ein Vector of SearchResult
	using searchList = std::vector<SearchResult>;
	/**
	* Suche nach Pattern im Text mit hilfe von RegEx
	*
	* @param	text: Der Text der nach dem Muster durchsucht werden soll
	*			pattern: Gibt das Muster an nach dem gesucht werden soll
	* @return	Gibt einen Vector of SearchResult zurück. Enthält alle gefundenen string mit ihrere jeweiligen Position im Text.
	*/
	searchList findPatterns(const std::string& text, const std::regex& pattern);

	/**
	* Berechnet die durchschnittliche Wortlänge
	*
	* @param	text: Der Text zum berechnen
	* @return	Gibt die durchschnittliche Wortlänge aus
	*/
	double calculateWordLength(const std::string& text);

	/**
	* Berechnet die durchschnittliche Satzlänge anhand von Wörtern
	*
	* @param	text: Der Text zum berechnen
	* @return	Gibt die durchschnittliche Wortanzahl pro Satz aus
	*/
	double calculateSentenceLengthByWords(const std::string& text);

	/**
	* Berechnet die durchschnittliche Satzlänge anhand von Zeichen
	*
	* @param	text: Der Text zum berechnen
	* @return	Gibt die durchschnittliche Zeichenanzahl pro Satz aus
	*/
	double calculateSentenceLengthByLetters(const std::string& text);

	/**
	* Berechnet die durchschnittliche Wortlängen verteilung. Also wie viele Worte gibt es von einer bestimmten länge
	*
	* @param	text: Der Text zum berechnen
	*			endingPattern (defult = defaultWordEnd " .,-!?\n"): Damit wird definiert wann ein Wort zu ende ist und ein neues Wort beginnt.
	* @return	Gibt eine map of int int zurück. Key ist die Anzahl der Buchstaben im Wort und der Value ist die Anzahl der Worte mit der selben Buchstaben zahl.
	*/
	wordLengthDistibution calculateWordLengthDistibution(const std::string& text, const std::string& endingPattern = defaultWordEnd);

	/**
	* Async version
	*/
	wordLengthDistibution calculateWordLengthDistibutionAsync(const std::string& text, const std::string& endingPattern = defaultWordEnd);

	/// ein string tuple um 2 Worte zu verknüpfen
	using strPair = std::pair<std::string, std::string>;
	/**
	* Erzeugt ein Hash aus zwei strings.
	* Dieses Struct wird benötigt damit strPair als Key für die map genutzt werden kann.
	*/
	struct PairHash
	{
		size_t operator()(const strPair& o) const
		{
			size_t h1 = std::hash<std::string>{}(o.first);
			size_t h2 = std::hash<std::string>{}(o.second);
			return h1 ^ (h2 << 1);
		}
	};
	// Eine Map mit einem Tuple als key und einem eigenem struct um aus dem tuple ein hash zu generieren
	using wordPairs = std::unordered_map<strPair, int, PairHash>;
	/**
	* Findet herraus welche Worte in einem Satz zusammen auftreten in einer festgelegten Reichweite.
	* Kann verwendet werden für eine "Ko - Okkurrenz Analyse".
	* 
	* @param	text: Der Text zum durchsuchen
	*			minOccurrences (default=2): Legt fest wie oft eine Paarung auftreten muss damit es in der Ausgabe landet.
	*			range (default=2): Legt die reichweite fest in der Paarungen auftreten können. Ein Wert unter 2 ist nicht sinnvoll.
	* @return	Gibt eine Unordered Map of strPair int zurück. Der Key besteht aus einem string paar, die beiden Worte die in einem Satz vorgekommen sind.
	*			Der Value ist die Anzahl wie oft die Kombination vorgekommen ist.
	*/
	wordPairs findConnectedWords(const std::string& text, int minOccurrences = 2, int range = 2);

	/**
	* Async version
	*/
	wordPairs findConnectedWordsAsync(const std::string& text, int minOccurrences = 2, int range = 2);

	/**
	* Ein enum für die bessere Einteilung des Lesbarkeitswertes
	*/
	enum ReadabilityScore
	{
		VeryEasy,     // 90–100: Suitable for an 11-year-old student
		Easy,         // 80–90
		MediumEasy,   // 70–80
		Medium,       // 60–70: Suitable for students aged 13–15
		MediumHard,   // 50–60
		Hard,         // 30–50
		VeryHard      // 0–30: Suitable for academics
	};
	/**
	 * Berechnet den Lesbarkeitswert eines Textes anhand des "Flesch - Reading - Ease - Score".
	 * Die Berechnung bezieht sich auf deutsche wörter und Texte.
	 *
	 * @param	text: Der Text zum berechnen des Scores
	 * @return	Gibt den Score als ReadabilityScore Enum zurück.
	 */
	ReadabilityScore calculateGermanScore(const std::string& text);

	/**
	* Konvertiert das ReadabilityScore enum in einen String
	* 
	* @param	s: Das enum was dargestellt werden soll
	* @return	Gibt den passenden String für das ReadabilityScore Enum zurück.
	*/
	inline std::string scoreToString(ReadabilityScore s)
	{
		switch (s)
		{
		case VeryEasy: return "VeryEasy";     
		case Easy: return "Easy";         
		case MediumEasy: return "MediumEasy";
		case Medium: return "Medium";      
		case MediumHard: return "MediumHard";
		case Hard: return "Hard";         
		case VeryHard: return "VeryHard";
		}
		return "Not defined";
	};

	/**
	* Räumt den Cache auf der inter bei dieser Bibliothek erstellt wird
	*/
	void clearCache();
}

#endif // !TEXTANALYSER
