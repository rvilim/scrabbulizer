
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
#include "scrabble.h"

std::array<int, 26> gamecounts;
Trie *dict;
std::vector<struct_word> wordinfo_points;

int main() {

    auto dictionary_path = "/enable1.txt";
    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
    dict = Trie::CreateFromFile(dictionary_path);

    ScoreDictionary(dictionary_path);

    std::vector<int> blanks;
    struct_word arrangement = {"", 0, gamecounts, blanks};

    std::array<int, 26> remainingletters = gamecounts;

    std::array<int, 26> trimmedcount = {};

    std::vector<struct_word> bestwords;


    auto a = GetTopAdditions({"", 0, gamecounts}, gamecounts, 0);
    std::cout<<"Leaders"<<std::endl;
    for(int i=0; i<a.size(); i++){
        std::cout<<a[i].score<< " "<<a[i].word<<std::endl;
    }
}
