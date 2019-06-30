#define COUNTALLWORDS ScoreAllWords_dedupe
#define DEBUG true

#include <iostream>
#include "trie.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <array>
#include <vector>
#include <algorithm>
#include <iterator>
#include "robin_hood.h"
#include <set>
#include <queue>


struct struct_word {
    std::string word;
    int score;
    std::array<int, 26> letters;
};

std::array<int, 26> gamecounts = {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};

Trie *dict;
Trie *dict_reversed;

std::unordered_map<std::string, struct_word> wordinfo_letters;
std::vector<struct_word> wordinfo_points;

int ScoreWord(std::string s);

void CountLetters(std::string s, std::array<int, 26> &count);

bool CompareWordPoints(const struct_word &a, const struct_word &b);

void ScoreDictionary(const std::string &dictionary_path);

struct_word BestAddition(struct_word baseword, std::array<int, 26> &remaining_letters);

std::array<int, 26> SubtractLetters(std::array<int, 26> a, std::array<int, 26> b);

int ScoreAllWords(std::string s);

int ScoreAllWords_std(std::string s);

int ScoreAllWords_dedupe(std::string s);

int AddWordOverlap(std::string &baseword, std::string &newword);

bool CanAddWord(std::array<int, 26> remaining_letters, std::array<int, 26> word_letters);

void PrintWord(struct_word w);

template<typename Iter>
Iter removeDuplicates(Iter begin, Iter end);

int main() {
    //                                a b c d e  f g h i j k l m n o p q r s t u v w x y z

    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/enable1.txt";
    auto dictionary_reversed_path = "/Users/rvilim/repos/scrabbulizer/enable1_reversed.txt";

    dict = Trie::CreateFromFile(dictionary_path);
    dict_reversed = Trie::CreateFromFile(dictionary_reversed_path);

    ScoreDictionary(dictionary_path);

    struct_word arrangement = {"", 0, gamecounts};
    std::array<int, 26> remainingletters = gamecounts;

    std::array<int, 26> trimmedcount = {};

//    std::string a="hello";
//    std::string b="hi";
//    auto overlap=AddWordOverlap(a,b);
//    std::cout<<a.substr(0,a.length()-overlap)<<"_"<<b<<std::endl;

//    b="odasfa";
//    auto overlap=AddWordOverlap(a,b);
//    std::cout<<overlap<<" "<<a.substr(0,a.length()-overlap)<<"_"<<b<<std::endl;

//    b="olledf";
//    overlap=AddWordOverlap(a,b);
//    std::cout<<overlap<<" "<<a.substr(0,a.length()-overlap)<<"_"<<b<<std::endl;




    std::string prevword = arrangement.word;
    //Exit our loop if the word did not change, e.g. the best addition did not add anything.
    while ((arrangement.word != prevword) || (prevword == "")) {
        prevword = arrangement.word;
        arrangement = BestAddition(arrangement, remainingletters);
        remainingletters = SubtractLetters(gamecounts, arrangement.letters);
    }
}

void ScoreDictionary(const std::string &dictionary_path) {

    std::string s;
    std::ifstream f(dictionary_path);

    while (std::getline(f, s)) {
        int score = COUNTALLWORDS(s);

        auto wordcount = std::array<int, 26>();
        CountLetters(s, wordcount);
        auto word = struct_word{s, score, wordcount};

        wordinfo_letters.insert(std::make_pair(s, word));
        wordinfo_points.push_back(word);
    }
    std::sort(wordinfo_points.begin(), wordinfo_points.end(), CompareWordPoints);

}

int AddWordOverlap(std::string &baseword, std::string &newword) {
    // Returns the number of letters of overlap

    unsigned long lenbaseword = baseword.length();
    unsigned long lennewword = newword.length();
    for (int i = 1; i < baseword.length(); i++) {
        int j = 0;

        while ((baseword[i + j] == newword[j]) && (j < lennewword) && (i + j < lenbaseword)) {
            j++;
        }

        if (i + j == lenbaseword) {
            return j;
        }
    }
    return 0;
//    return minlength;
}

struct_word BestAddition(struct_word baseword, std::array<int, 26> &remaining_letters) {
    std::array<int, 26> trimmedcount = {};

    struct_word bestword = baseword;
    if (bestword.word.length() == 0) {
        bestword.word = 'a';
    }

    float bestefficiency = 0.0;


    for (auto word = begin(wordinfo_points); word != end(wordinfo_points); ++word) {
        int overlap = 0;
        std::string trimmedword = "";

        // Try to add the word to the end
        overlap = AddWordOverlap(baseword.word, word->word);
        if (overlap == word->word.length()) continue;

        trimmedword = (word->word).substr(overlap, (word->word).length() - overlap);

        trimmedcount.fill(0);
        CountLetters(trimmedword, trimmedcount);

        if (CanAddWord(remaining_letters, trimmedcount)) {
            std::string w = baseword.word + trimmedword;
            int s = COUNTALLWORDS(w);

            if (s > bestword.score) {
//            if ((float) s / (float) w.length() > bestefficiency) {
                bestefficiency = (float) s / (float) w.length();
                std::array<int, 26> l{};
                CountLetters(w, l);

                bestword = struct_word{w, s, l};
                if (DEBUG) PrintWord(bestword);
            }
        }

        if (baseword.word == "") continue;

        // Try to add the word to the start
        overlap = AddWordOverlap(word->word, baseword.word);
        if (overlap == word->word.length()) continue;

        trimmedword = word->word.substr(0, word->word.length() - overlap);



        trimmedcount.fill(0);
        CountLetters(trimmedword, trimmedcount);

        if (CanAddWord(remaining_letters, trimmedcount)) {
            std::string w = trimmedword + baseword.word;

            int s = COUNTALLWORDS(w);
            if (s > bestword.score) {
//            if ((float) s / (float) w.length() >= bestefficiency) {
                bestefficiency = (float) s / (float) w.length();
                std::array<int, 26> l{};
                CountLetters(w, l);

                bestword = struct_word{w, s, l};
                if (DEBUG) PrintWord(bestword);
            }
        }
    }
    return bestword;
}

void PrintWord(struct_word w) {
    std::cout << w.word << ", " << w.score << ", " << (float) w.score / (float) w.word.size() << ", ";
    for (int i = 0; i < 26; i++) {
        std::cout << SubtractLetters(gamecounts, w.letters)[i] << " ";
    }
    std::cout << std::endl;

}

std::array<int, 26> SubtractLetters(std::array<int, 26> a, std::array<int, 26> b) {
    std::array<int, 26> r;
    for (int i = 0; i < 26; i++) {
        r[i] = a[i] - b[i];
    }
    return r;
}

bool CanAddWord(std::array<int, 26> remaining_letters, std::array<int, 26> word_letters) {
    std::array<int, 26> new_letters{};
    int n_blanks = 0;
    for (int i = 0; i < 26; i++) {
        new_letters[i] = remaining_letters[i] - word_letters[i];

        if (new_letters[i] < 0) {
            n_blanks -= new_letters[i];
            if (n_blanks > 2) {
                return false;
            }
        }
    }
    return true;
}

void PrintArray(std::array<int, 26> &a) {
    for (int i = 0; i < 26; i++) {
        std::cout << a[i] << " ";
    }
}

void CountLetters(std::string s, std::array<int, 26> &count) {
    for (char &c : s) {
        count[(int) c - (int) 'a'] += 1;
    }
}

template<typename Iter>
Iter removeDuplicates(Iter begin, Iter end) {
    auto it = begin;
    while (it != end) {
        auto next = std::next(it);
        if (next == end) {
            break;
        }
        end = std::remove(next, end, *it);
        it = next;
    }

    return end;
}

int ScoreAllWords_dedupe(std::string s) {
    int score = ScoreWord(s);
    int n = s.length();

    char char_array[n];
    strcpy(char_array, s.c_str());

    std::vector<std::pair<std::string, bool>> words;
    words.reserve(s.length());

    for (int start = 0; start < s.length() - 2; start++) {
        dict->WordsStartWith(char_array + start, 0, words);
    }

    words.erase(removeDuplicates(words.begin(), words.end()), words.end());

    for (auto it = words.begin(); it != words.end(); ++it) {
        score += ScoreWord(it->first);
    }

    return score;
}

int ScoreAllWords(std::string s) {
    robin_hood::unordered_flat_map<std::string, bool> foundwords;
    int score = ScoreWord(s);
    int n = s.length();

    char char_array[n];
    strcpy(char_array, s.c_str());

    std::vector<std::pair<std::string, bool>> words;
    words.reserve(s.length());

    for (int start = 0; start < s.length() - 2; start++) {
        dict->WordsStartWith(char_array + start, 0, words);
    }

    foundwords.insert(words.begin(), words.end());

    for (auto it = foundwords.begin(); it != foundwords.end(); ++it) {
        score += ScoreWord(it->first);
    }

    return score;
}

int ScoreAllWords_std(std::string s) {
    std::unordered_map<std::string, bool> foundwords;
    int score = ScoreWord(s);
    int n = s.length();

    char char_array[n];
    strcpy(char_array, s.c_str());

    std::vector<int> wordends;
    wordends.reserve(s.length());

    for (int start = 0; start < s.length() - 2; start++) {

        dict->WordsStartWith_pos(char_array + start, 0, wordends);

        for (int wordend : wordends) {
            auto word = s.substr(start, wordend);

            if (foundwords.find(word) == foundwords.end()) {
                foundwords.insert(std::make_pair(word, true));
                score += ScoreWord(word);
            }

        }
        wordends.clear();
    }
    return score;
}

int ScoreWord(std::string s) {
    //                       a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q   r  s  t  u  v  w  x  y  z
    const int letters[26] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};
    int score = 0;

    for (char &c : s) {
        score += letters[(int) c - (int) 'a'];
    }

    return score;
}

bool CompareWordPoints(const struct_word &a, const struct_word &b) {
    return a.score < b.score;
}