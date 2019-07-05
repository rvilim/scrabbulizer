//
// Created by Ryan Vilim on 2019-07-04.
//
#include "scrabble.h"

std::vector<struct_word> GetTopAdditions(struct_word word, std::array<int, 26> remainingletters, int num){
    std::vector<struct_word> bestwords = BestAddition(word, remainingletters);
    std::vector<struct_word> r;

    if (bestwords.size()==0){
        r.push_back(word);
    }

    for(auto & bestword : bestwords){
        std::cout<<"level "<<num<<std::endl;

        auto l = SubtractLetters(gamecounts, bestword.letters);
        auto a = GetTopAdditions(bestword, l, num+1);

        r.insert(r.end(), a.begin(), a.end());
    }

    return r;
}
void AddWordToHeap(std::vector<struct_word> &bestwords, struct_word newword, const int heapsize){

    if ((bestwords.size()<heapsize)|| ((bestwords.size()>=heapsize) && (bestwords.front().score<newword.score))){
        bestwords.push_back(newword); std::push_heap (bestwords.begin(),bestwords.end(), minheap());

        if(bestwords.size()>heapsize){
            std::pop_heap(bestwords.begin(), bestwords.end(), minheap()); bestwords.pop_back();
        }
        for(int i=0; i<bestwords.size(); i++){
            if (DEBUG) PrintWord(bestwords[i]);
        }
        std::cout<<"*****"<<std::endl;
        std::cout<<std::endl;

    }

}
void ScoreDictionary(const std::string &dictionary_path) {

    std::string s;
    std::ifstream f(dictionary_path);

    while (std::getline(f, s)) {
        int score = ScoreAllWords_dedupe(s);

        auto wordcount = std::array<int, 26>();
        CountLetters(s, wordcount);
        auto word = struct_word{s, score, wordcount};

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
}

void AddWord(struct_word & baseword, struct_word & word, const std::array<int, 26> &remaining_letters,
             std::vector<struct_word> &bestwords, const int &n_candidates, bool append){
    int overlap = 0;
    std::string trimmedword;
    std::array<int, 26> trimmedcount = {};

    //If this is the first word, then trimmedword = the word we are looking at
    trimmedword=word.word;

    //Try to add it to the end
    if (baseword.word.length()!=0) {
        overlap = AddWordOverlap(baseword.word, word.word);
        if (overlap == word.word.length()) return;

        trimmedword = (word.word).substr(overlap, (word.word).length() - overlap);

        trimmedcount.fill(0);
    }

    CountLetters(trimmedword, trimmedcount);

    // Try to add the word to the end
    if (CanAddWord(remaining_letters, trimmedcount)) {
        std::string w;

        if (append){
            w = baseword.word + trimmedword;
        }else{
            w = trimmedword + baseword.word;
        }

        int s = ScoreAllWords_dedupe(w);
        std::cout<<w<<"---"<<s<<std::endl;
        std::array<int, 26> l{};
        CountLetters(w, l);
        auto q = struct_word{w, s, l};
        AddWordToHeap(bestwords, q, n_candidates);
    }
}

std::vector<struct_word> BestAddition(struct_word baseword, const std::array<int, 26> &remaining_letters) {
    const int n_candidates = 4;
    std::vector<struct_word> bestwords;

    for (auto word = begin(wordinfo_points); word != end(wordinfo_points); ++word) {
        AddWord(baseword, *word, remaining_letters, bestwords, n_candidates, true);
        AddWord(*word, baseword, remaining_letters, bestwords, n_candidates, true);
    }
    return bestwords;
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
    int score = 0;
    int n = s.length();

    char char_array[n];
    strcpy(char_array, s.c_str());

    std::vector<std::string> words;
    words.reserve(s.length());

    for (int start = 0; start <= s.length() - 2; start++) {
        dict->WordsStartWith(char_array + start, 0, words);
    }

    words.erase(removeDuplicates(words.begin(), words.end()), words.end());

    for (auto it = words.begin(); it != words.end(); ++it) {
        score += ScoreWord(*it);
    }

    return score;
}

int ScoreWord(std::string &s) {
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

/*
 *        // Try to add the word to the start
        overlap = AddWordOverlap(word->word, baseword.word);
        if (overlap == word->word.length()) continue;

        trimmedword = word->word.substr(0, word->word.length() - overlap);

        trimmedcount.fill(0);
        CountLetters(trimmedword, trimmedcount);

        if (CanAddWord(remaining_letters, trimmedcount)) {
            std::string w = trimmedword + baseword.word;

            int s = ScoreAllWords_dedupe(w);
            if (s > bestword.score) {
//            if ((float) s / (float) w.length() >= bestefficiency) {
                bestefficiency = (float) s / (float) w.length();
                std::array<int, 26> l{};
                CountLetters(w, l);

                bestword = struct_word{w, s, l};
                AddWordToHeap(bestwords, bestword, n_candidates);
                if (DEBUG) PrintWord(bestword);
            }
        }
 */