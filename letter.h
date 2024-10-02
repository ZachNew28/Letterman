// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "getopt.h"

struct DictionaryEntry {
    string dictWord;
    int prev = -1;
    bool discovered = false;
};

class Letterman {
private:
    // Member variables
    std::vector<DictionaryEntry> Dictionary;
    size_t dictSize = 0;
    string beginWord;
    string endWord;
    int beginWordIdx = 0;
    int endWordIdx = 0;
    bool isSimple = false;
    bool isComplex = false;
    bool stackMode = false;
    bool queueMode = false;
    bool morphChange = false;
    bool morphSwap = false;
    bool morphLength = false;   // Insert and Delete
    bool wordOutput = false;
    bool modOutput = false;
    bool searchFailed = false;
    // Member functions
    void readSimple();
    void readComplex();
    bool letterChange(const DictionaryEntry& currentWord, const DictionaryEntry& newWord);
    bool letterLength(const DictionaryEntry& currentWord, const DictionaryEntry& newWord);
    bool letterSwap(const DictionaryEntry& currentWord, const DictionaryEntry& newWord);

public:
    Letterman() = default;
    void getOptions(int argc, char* argv[]);
    void readDictionary();
    void search();   // Make and utilize a deque in this
    void output();
    bool morphWord(const DictionaryEntry& currentWord, const DictionaryEntry& newWord);
};
