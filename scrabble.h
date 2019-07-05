//
// Created by Ryan Vilim on 2019-07-04.
//
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


#ifndef SCRABBULIZER_SCRABBLE_H
#define SCRABBULIZER_SCRABBLE_H

#endif //SCRABBULIZER_SCRABBLE_H

struct struct_word {
    std::string word;
    int score;
    std::array<int, 26> letters;
};

struct minheap{
    bool operator()(const struct_word& a,const struct_word& b) const{
        return a.score>b.score;
    }
};

extern std::array<int, 26> gamecounts;
extern Trie *dict;
extern std::vector<struct_word> wordinfo_points;

int ScoreWord(std::string & s);
void CountLetters(std::string s, std::array<int, 26> &count);
bool CompareWordPoints(const struct_word &a, const struct_word &b);
void ScoreDictionary(const std::string &dictionary_path);
std::vector<struct_word>  BestAddition(struct_word baseword, const std::array<int, 26> &remaining_letters);
std::array<int, 26> SubtractLetters(std::array<int, 26> a, std::array<int, 26> b);
int ScoreAllWords_dedupe(std::string s);
int AddWordOverlap(std::string &baseword, std::string &newword);
bool CanAddWord(std::array<int, 26> remaining_letters, std::array<int, 26> word_letters);
void PrintWord(struct_word w);
template<typename Iter> Iter removeDuplicates(Iter begin, Iter end);
void AddWordToHeap(std::vector<struct_word> &bestwords, struct_word newword, const int heapsize);
void AddWord(struct_word & baseword, struct_word & word, const std::array<int, 26> &remaining_letters,
             std::vector<struct_word> &bestwords, const int & n_candidates, bool append);
std::vector<struct_word> GetTopAdditions(struct_word word, std::array<int, 26> remainingletters, int num);
