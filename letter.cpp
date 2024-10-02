// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include "letter.h"

#include <cstddef>
#include <vector>

#include <getopt.h>

// WHEN DEBUGGING, MIGHT NEED TO PUT A BREAKPOINT SOMEWHERE (I THINK IN MAIN)

int main(int argc, char* argv[]) {
    // Speed up io
    ios_base::sync_with_stdio(false);

    Letterman letter;

    letter.getOptions(argc, argv);
    letter.search();
    letter.output();
}

// Prints a help message if requested that explains all the options and what the program does
// argv[0] is the name of the program
void printHelp(char* argv[]) {
    cout << "Usage: " << argv[0] << " -h | -q | -s | [-b <word>] | [-e <word>] | ";
    cout << "[-o (W|M)] | -c | -l | -p\n";
    cout << "This program is used to convert a \"beginning\" word into an \"end\" word\n";
    cout << "while only allowing specific single character conversions.\n\n";
    cout << "-h: Prints a help message the explains the program and options\n";
    cout << "-q: Directs the program to use a search container that behaves like a queue\n";
    cout << "-s: Directs the program to use a search container that behaves like a stack\n";
    cout << "-b <word>: Specifies the beginning word for the search\n";
    cout << "-e <word>: Specifies the end word for the search\n";
    cout << "-o (W|M): Specifies the output format, Word or Modification\n";
    cout << "-c: Allows Letterman to change one letter into another during word morphs\n";
    cout << "-l: Allows Letterman to insert or delete a single letter during word morphs\n";
    cout << "-p: Allows Letterman to swap any two adjacent letters during word morphs" << endl;
}

void Letterman::getOptions(int argc, char* argv[]) {
    opterr = false;
    int choice;
    int index = 0;
    bool wordFound = false;

    // These are all the options
    // Implement: help, queue, stack, begin, end, output, change, length, swap
    option long_options[] = {
        {  "help",       no_argument, nullptr,  'h'},
        { "queue",       no_argument, nullptr,  'q'},
        { "stack",       no_argument, nullptr,  's'},
        {"change",       no_argument, nullptr,  'c'},
        {"length",       no_argument, nullptr,  'l'},
        {  "swap",       no_argument, nullptr,  'p'},
        { "begin", required_argument, nullptr,  'b'},
        {   "end", required_argument, nullptr,  'e'},
        {"output", required_argument, nullptr,  'o'}, //  legal requirements for output are W or M
        { nullptr,                 0, nullptr, '\0'},
    };

    readDictionary();

    // Actually get the desired option now and do something with it
    // MIGHT NOT BE DONE
    while ((choice = getopt_long(argc, argv, "hqsclpb:e:o:", long_options, &index)) != -1) {
        if (choice != 'h' && choice != 'q' && choice != 's' && choice != 'c' && choice != 'l' && choice != 'p'
            && choice != 'b' && choice != 'e' && choice != 'o') {
            cerr << "Unknown command line option" << endl;
            exit(1);
        }

        switch (choice) {
        case 'h':
            printHelp(argv);
            exit(0);

        case 'q':
            queueMode = true;
            break;

        case 's':
            stackMode = true;
            break;

        case 'b': {
            string arg { optarg };
            beginWord = arg;

            // See if the word is in the dictionary
            for (size_t i = 0; i < Dictionary.size(); ++i) {
                if (Dictionary[i].dictWord == arg) {
                    wordFound = true;
                    break;
                }
            }
            // If the word wasn't found, error
            if (!wordFound) {
                cerr << "Beginning word does not exist in the dictionary" << endl;
                exit(1);
            }

            break;
        }

        case 'e': {
            string arg { optarg };
            endWord = arg;

            for (size_t i = 0; i < Dictionary.size(); ++i) {
                if (Dictionary[i].dictWord == arg) {
                    wordFound = true;
                    break;
                }
            }
            // If the word wasn't found, error
            if (!wordFound) {
                cerr << "Ending word does not exist in the dictionary" << endl;
                exit(1);
            }

            break;
        }

        case 'o': {
            string arg { optarg };
            if (arg != "W" && arg != "M") {
                cerr << "Invalid output mode specified" << endl;
                exit(1);
            }

            if (arg == "M") {
                modOutput = true;
            } else {
                wordOutput = true;
            }

            break;
        }

        case 'c':
            morphChange = true;
            break;

        case 'l':
            morphLength = true;
            break;

        case 'p':
            morphSwap = true;
            break;
        }
    }

    // If length mode is not enabled delete all the smaller words
    if (!morphLength) {
        dictSize = Dictionary.size();
        size_t beginSize = beginWord.size();
        for (size_t i = 0; i < Dictionary.size(); ++i) {
            if (Dictionary[i].dictWord.size() != beginSize) {
                Dictionary.erase(Dictionary.begin() + int(i));
            }
        }
    }
    dictSize = Dictionary.size();

    // Error messages and set deafult output mode to word output
    if (stackMode && queueMode) {
        cerr << "Conflicting or duplicate stack and queue specified" << endl;
        exit(1);
    }
    if (!stackMode && !queueMode) {
        cerr << "Must specify one of stack or queue" << endl;
        exit(1);
    }
    if (!morphChange && !morphLength && !morphSwap) {
        cerr << "Must specify at least one modification mode (change length swap)" << endl;
        exit(1);
    }
    if ((beginWord.size() != endWord.size()) && !morphLength) {
        cerr << "The first and last words must have the same length when length mode is off" << endl;
        exit(1);
    }
    if (!wordOutput && !modOutput) {
        wordOutput = true;
    }
}


// This function will read the dictionary, determine if it is simple or complex, and store the data
// Use a while loop with cin as the bool statement
void Letterman::readDictionary() {
    DictionaryEntry entry;
    string complexity;
    std::vector<DictionaryEntry>::size_type numWords;

    // Read in the complexity and the number of words in the dictionary
    cin >> complexity;
    cin >> numWords;

    Dictionary.reserve(numWords);


    // Determine if it is simple or complex and change the bools in DictionaryEntry for complex entries
    if (complexity == "S") {
        isSimple = true;
        readSimple();
    } else {
        isComplex = true;
        readComplex();
    }
}

void Letterman::readSimple() {
    string words;
    DictionaryEntry entry;

    // Read in the dictionary and store the entries while deleting the comments
    while (cin >> words) {
        if (words == "//") {   // Ignore all of the comments in the file
            cin.ignore(256, '\n');
        } else {
            entry.dictWord = words;
            Dictionary.push_back(entry);
        }
    }
}

void Letterman::readComplex() {
    string words;
    DictionaryEntry entry;
    string tempWord;
    char storeLetter1;
    char storeLetter2;
    int letterPosition;
    string doubleLetter;

    // Read in the dictionary and store the entries while deleting the comments
    while (cin >> words) {
        if (words.find('/') != std::string::npos) {   // Ignore all of the comments in the file
            cin.ignore(256, '\n');
        }
        // Reverse
        else if (words.find('&') != std::string::npos) {
            words.pop_back();
            entry.dictWord = words;
            Dictionary.push_back(entry);
            std::reverse(words.begin(), words.end());
            entry.dictWord = words;
            Dictionary.push_back(entry);
        }
        // Insert-each
        else if (words.find('[') != std::string::npos) {
            tempWord = words;
            int delLetters = 0;

            // Create all the necessary words and add them
            for (std::vector<DictionaryEntry>::size_type i = words.find('[') + 1; i < words.find(']'); ++i) {
                words.erase(words.begin() + int(words.find('[')),
                            words.begin() + int(i));   // THIS MIGHT GIVE PROBLEMS, FIX IF IT DOES (MAYBE CAST TO INT?)
                words.erase(words.begin() + int(i) - delLetters, words.begin() + int(words.find(']')) + 1);
                delLetters++;
                entry.dictWord = words;
                Dictionary.push_back(entry);   // Add the word after deleting the letters around it, including []
                words = tempWord;              // Reset the word
            }
        }
        // Swap
        else if (words.find('!') != std::string::npos) {
            tempWord = words;
            storeLetter1 = words[words.find('!') - 2];   // Store the left most letter to swap
            storeLetter2 = words[words.find('!') - 1];   // Store the oother letter

            words.erase(words.begin() + int(words.find('!')));   // Might give same problem as Insert
            entry.dictWord = words;
            Dictionary.push_back(entry);   // Add first word
            words = tempWord;              // Reset to add second word

            words[words.find('!') - 2] = storeLetter2;   // Replace the first character with the second
            words[words.find('!') - 1] = storeLetter1;   // Replace the second character with the first
            words.erase(words.begin() + int(words.find('!')));
            entry.dictWord = words;
            Dictionary.push_back(entry);   // Store the second word
        }
        // Double
        else if (words.find('?') != std::string::npos) {
            // THINGS MIGHT GO WRONG IF THE '?' IS THE VERY LAST CHARACTER
            letterPosition = int(words.find('?'));   // ONCE AGAIN FIND MIGHT CAUSE ERRORS
            doubleLetter = words[static_cast<std::string::size_type>(letterPosition - 1)];

            words.erase(words.begin() + int(words.find('?')));
            entry.dictWord = words;
            Dictionary.push_back(entry);   // Add first word
            words.insert(static_cast<std::string::size_type>(letterPosition), doubleLetter);
            entry.dictWord = words;
            Dictionary.push_back(entry);   // Add second letter
        }
        // Normal word
        else {
            entry.dictWord = words;
            Dictionary.push_back(entry);
        }
    }
}

void Letterman::search() {
    deque<int> searchContainer;   // Made of ints so that we just keep track of indices
    int currentWordIdx;
    bool endFound = false;

    // Get the index for the beginning word and end word
    for (size_t i = 0; i < dictSize; ++i) {
        if (Dictionary[i].dictWord == beginWord) {
            beginWordIdx = static_cast<int>(i);
        }
        if (Dictionary[i].dictWord == endWord) {
            endWordIdx = static_cast<int>(i);
        }
    }

    // Case where the begin word and end word are the same
    if (beginWordIdx == endWordIdx) {
        return;
    }

    searchContainer.push_front(beginWordIdx);   // Add index of first word to search container
    Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(beginWordIdx)].discovered
      = true;   // Mark the first word as discovered


    if (queueMode) {   // Push back and pop front
        // Loop through the search container and add any similar words to the curent word
        while (!searchContainer.empty()) {
            currentWordIdx
              = searchContainer[0];   // Because we delete the old word, the new current word will always be at idx 0
            searchContainer.pop_front();

            // Loop through the dictionary and add the similar words
            for (size_t j = 0; j < dictSize; ++j) {
                if (!Dictionary[j].discovered) {   // If the word hasn't been discovered, see if you can morph it
                    if (morphWord(Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(currentWordIdx)],
                                  Dictionary[j])) {
                        Dictionary[j].discovered = true;
                        Dictionary[j].prev = currentWordIdx;
                        searchContainer.push_back(static_cast<int>(j));
                        // If the end word was found then kill the loop
                        if (Dictionary[j].dictWord
                            == Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(endWordIdx)].dictWord) {
                            endFound = true;
                            break;
                        }
                    }
                }
            }

            // If the end word was found then kill the loop
            if (endFound) {
                break;
            }
        }
        if (!endFound) {
            searchFailed = true;
        }
    }
    // Stack based
    else {
        while (!searchContainer.empty()) {
            currentWordIdx
              = searchContainer[searchContainer.size() - 1];   // Because we delete the old word, the new current word
                                                               // will always be at the last idx
            searchContainer.pop_back();

            // Loop through the dictionary and add the similar words
            for (size_t j = 0; j < dictSize; ++j) {
                if (!Dictionary[j].discovered) {   // If the word hasn't been discovered, see if you can morph it
                    if (morphWord(Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(currentWordIdx)],
                                  Dictionary[j])) {
                        Dictionary[j].discovered = true;
                        Dictionary[j].prev = currentWordIdx;
                        searchContainer.push_back(static_cast<int>(j));
                        // If the end word was found then kill the loop
                        if (Dictionary[j].dictWord
                            == Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(endWordIdx)].dictWord) {
                            endFound = true;
                            break;
                        }
                    }
                }
            }

            // If the end word was found then kill the loop
            if (endFound) {
                break;
            }
        }
        if (!endFound) {
            searchFailed = true;
        }
    }
}

// Use this function to see if the word in the dicitonary is similar to the current word
// MIGHT NEED TO MAKE A FUNCTION FOR EACH CHANGE MODE BUT IDK YET
// THIS FUNCTION WILL TOTALLY CAUSE SOME PROBLEMS THAT NEED TO BE DEBUGGED KMS
bool Letterman::morphWord(const DictionaryEntry& currentWord, const DictionaryEntry& newWord) {
    // Go through the different morph modes and see if they can be done
    if (morphChange) {
        if (letterChange(currentWord, newWord)) {
            return true;
        }
    }
    if (morphLength) {
        if (letterLength(currentWord, newWord)) {
            return true;
        }
    }
    if (morphSwap) {
        if (letterSwap(currentWord, newWord)) {
            return true;
        }
    }

    return false;
}

void Letterman::output() {
    std::deque<int> indices;
    int curridx = endWordIdx;

    if (searchFailed) {
        int wordsDiscovered = 0;

        // Count up all the discovered words
        for (size_t i = 0; i < dictSize; ++i) {
            if (Dictionary[i].discovered) {
                wordsDiscovered++;
            }
        }

        cout << "No solution, " << wordsDiscovered << " words discovered." << endl;
    }
    // Search didn't fail
    else {
        int wordsMorphed = 1;
        // Add all indices to a deque so that I can easily print the words and count morphs
        while (curridx != beginWordIdx) {
            indices.push_front(curridx);
            curridx = Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(curridx)].prev;
            wordsMorphed++;
        }
        indices.push_front(beginWordIdx);
        cout << "Words in morph: " << wordsMorphed << endl;

        if (wordOutput) {
            for (size_t i = 0; i < indices.size(); ++i) {
                cout << Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(indices[i])].dictWord << endl;
            }
        } else {
            int position = 0;
            DictionaryEntry firstEntry;
            DictionaryEntry secondEntry;
            char newLetter = 'c';

            cout << Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(beginWordIdx)].dictWord << endl;

            // Loop through and check each of the change modes and say what they do
            for (size_t i = 0; i < indices.size() - 1; ++i) {
                bool foundLetter = false;   // Reset this for deletions

                firstEntry = Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(indices[i])];
                secondEntry = Dictionary[static_cast<std::vector<DictionaryEntry>::size_type>(indices[i + 1])];

                if (letterChange(firstEntry, secondEntry)) {
                    // Loop through the words and find the position of the changed letter
                    for (size_t j = 0; j < firstEntry.dictWord.size(); ++j) {
                        if (firstEntry.dictWord[j] != secondEntry.dictWord[j]) {
                            position = static_cast<int>(j);
                            newLetter = secondEntry.dictWord[j];
                            break;
                        }
                    }

                    cout << "c," << position << "," << newLetter << endl;
                } else if (letterLength(firstEntry, secondEntry)) {
                    // Letter was inserted
                    if (firstEntry.dictWord.size() < secondEntry.dictWord.size()) {
                        bool letterFound = false;

                        // Loop through the words and find the position of the changed letter
                        for (size_t j = 0; j < firstEntry.dictWord.size(); ++j) {
                            if (firstEntry.dictWord[j] != secondEntry.dictWord[j]) {
                                letterFound = true;
                                position = static_cast<int>(j);
                                newLetter = secondEntry.dictWord[j];
                                break;
                            }
                        }
                        // If the letter wasn't found in the loop, it means it was added to the end of the word
                        if (!letterFound) {
                            position = static_cast<int>(secondEntry.dictWord.size()) - 1;
                            newLetter = secondEntry.dictWord[static_cast<std::basic_string<char>::size_type>(position)];
                        }

                        cout << "i," << position << "," << newLetter << endl;
                    }
                    // Letter was deleted
                    else {
                        for (size_t j = 0; j < secondEntry.dictWord.size(); ++j) {
                            if (secondEntry.dictWord[j] != firstEntry.dictWord[j]) {
                                position = static_cast<int>(j);
                                foundLetter = true;
                                break;
                            }
                        }

                        if (!foundLetter) {
                            position = static_cast<int>(secondEntry.dictWord.size());
                        }

                        cout << "d," << position << endl;
                    }
                } else if (letterSwap(firstEntry, secondEntry)) {
                    // Loop through the words and find the position of the changed letter
                    for (size_t j = 0; j < firstEntry.dictWord.size(); ++j) {
                        if (firstEntry.dictWord[j] != secondEntry.dictWord[j]) {
                            position = static_cast<int>(j);
                            break;
                        }
                    }

                    cout << "s," << position << endl;

                } else {
                    cerr << "There is some sort of error in your code, TIME FOR DEBUGGING" << endl;
                    exit(1);
                }
            }
        }
    }
}

bool Letterman::letterChange(const DictionaryEntry& currentWord, const DictionaryEntry& newWord) {
    size_t currWordSize = currentWord.dictWord.size();
    size_t newWordSize = newWord.dictWord.size();

    // Loop through the letters of the current word and see how many letters are different
    if (newWordSize == currWordSize) {
        int numDiffLetters = 0;

        for (size_t i = 0; i < currWordSize; ++i) {
            if (newWord.dictWord[i] != currentWord.dictWord[i]) {
                numDiffLetters += 1;
            }
        }
        if (numDiffLetters == 1 && newWordSize == currWordSize) {
            return true;
        }
    }

    return false;
}

bool Letterman::letterLength(const DictionaryEntry& currentWord, const DictionaryEntry& newWord) {
    // Iterate through the characters of both words
    size_t currWordSize = currentWord.dictWord.size();
    size_t newWordSize = newWord.dictWord.size();

    if (currWordSize == newWordSize + 1 || currWordSize == newWordSize - 1) {
        int numEdits = 0;
        size_t i = 0;
        size_t j = 0;

        while (i < currWordSize && j < newWordSize) {
            // If characters are different, increment the edits counter
            if (currentWord.dictWord[i] != newWord.dictWord[j]) {
                numEdits++;

                // Check if more than one edit has been made
                if (numEdits > 1) {
                    return false;
                }

                // If lengths are different, move the index of the longer word
                if (currWordSize < newWordSize) {
                    j++;
                } else if (currWordSize > newWordSize) {
                    i++;
                } else {
                    i++;
                    j++;
                }
            } else {
                // Characters are the same, move both indices
                i++;
                j++;
            }
        }

        // Check for if the last letters are different
        if (i < currWordSize || j < newWordSize) {
            numEdits++;
        }

        // If there is exactly one edit or no edit at all, return true
        return numEdits == 1 || (numEdits == 0 && i < currWordSize && j < newWordSize);
    }

    return false;
}


bool Letterman::letterSwap(const DictionaryEntry& currentWord, const DictionaryEntry& newWord) {
    size_t currWordSize = currentWord.dictWord.size();
    size_t newWordSize = newWord.dictWord.size();

    if (newWordSize == currWordSize) {
        size_t resumeIdx = 0;   // Use this to keep track of where the supposed letter swap is
        bool contWork = false;

        // Loop through the words and find where the swap takes place
        for (size_t i = 0; i < newWordSize - 1; ++i) {
            if (currentWord.dictWord[i] == newWord.dictWord[i + 1]
                && currentWord.dictWord[i + 1] == newWord.dictWord[i]) {
                resumeIdx = i;
                contWork = true;
                break;
            }
        }

        if (contWork) {
            bool sameWord = true;

            // Find out if the rest of the letters in the words are the same
            for (size_t i = 0; i < resumeIdx; ++i) {
                if (currentWord.dictWord[i] != newWord.dictWord[i]) {
                    sameWord = false;
                    break;
                }
            }
            // Do the same for the rest of the word after the swap point and make sure the resumeIdx + 2 does not go
            // out of bounds
            if (sameWord && resumeIdx < currWordSize - 2) {
                for (size_t i = resumeIdx + 2; i < currWordSize; ++i) {
                    if (currentWord.dictWord[i] != newWord.dictWord[i]) {
                        sameWord = false;
                        break;
                    }
                }
            }
            if (sameWord) {
                return true;
            }
        }
    }

    return false;
}
