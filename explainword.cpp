#include <iostream>
#include <string>

#include "scrabble.h"

std::array<int, 26> gamecounts;
Trie *dict;
std::vector<struct_word> wordinfo_points;

int main(int argc, char *argv[]){
    auto dictionary_path = "/enable1.txt";
    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
    dict = Trie::CreateFromFile(dictionary_path);

    ScoreDictionary(dictionary_path);

    std::string word = argv[1];
    auto wordcount = std::array<int, 26>();

    CountLetters(word, wordcount);

    struct_word s = {
            word,
            0,
            wordcount,
            std::vector<int>{},
            0.0,
            0.0
    };
//    auto words = ScoreAllWords(s);

    PrintWordFull(s);
//    std::cout<<std::endl;
//    std::cout<<"Words:"<<std::endl;
//    for(int i=0; i<words.size(); i++){
//        std::cout<<words[i]<<std::endl;
//    }
}