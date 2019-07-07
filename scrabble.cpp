//
// Created by Ryan Vilim on 2019-07-04.
//
#include "scrabble.h"


std::vector<struct_word> GetTopAdditions(struct_word word, std::array<int, 26> remainingletters, int num) {
    std::vector<int> numlevel = {72, 10, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
    std::vector<float> mintileefficiency = {6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0};
    std::vector<struct_word> bestwords = BestAddition(word, remainingletters, numlevel[num], mintileefficiency[num]);
    std::vector<struct_word> r;
    struct_word bestscore = {"", 0, gamecounts, std::vector<int>{}};

    if (bestwords.size() == 0) {
        r.push_back(word);
    }
#pragma omp parallel
    {
#pragma omp for nowait //fill vec_private in parallel
        for (int i = 0; i < bestwords.size(); i++) {
            auto l = SubtractLetters(gamecounts, bestwords[i].letters);
            auto a = GetTopAdditions(bestwords[i], l, num + 1);

            for (int j = 0; j < a.size(); j++) {
#pragma omp critical
                {
                    if (a[j].score > bestscore.score) {
                        bestscore = a[j];
                        PrintWord(a[j]);
                    }
                }
            }
#pragma omp critical
            r.insert(r.end(), a.begin(), a.end());
        }
    }
    return r;
}

void AddWordToHeap(std::vector<struct_word> &bestwords, struct_word newword, const int heapsize, const float mintileefficiency) {

    if ((newword.tileefficiency > mintileefficiency) && ((bestwords.size() < heapsize) || ((bestwords.size() >= heapsize) &&
                                                                           (bestwords.front().score <
                                                                            newword.score)))) {
        bestwords.push_back(newword);
        std::push_heap(bestwords.begin(), bestwords.end(), minheap());

        if (bestwords.size() > heapsize) {
            std::pop_heap(bestwords.begin(), bestwords.end(), minheap());
            bestwords.pop_back();
        }
        for (int i = 0; i < bestwords.size(); i++) {
            if (DEBUG) PrintWord(bestwords[i]);
        }
    }

}


void ScoreDictionary(const std::string &dictionary_path) {

    std::string s;
    std::ifstream f(dictionary_path);

    while (std::getline(f, s)) {
        std::vector<int> blankpositions;
        auto wordcount = std::array<int, 26>();

        CountLetters(s, wordcount);

        auto word = struct_word{s, -1, wordcount, blankpositions};
        ScoreAllWords(word);

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

void AddWord(struct_word &baseword, struct_word &word, const std::array<int, 26> &remaining_letters,
             std::vector<struct_word> &bestwords, const int &n_candidates, const float & mintileefficiency, bool append) {
    int overlap = 0;
    std::string trimmedword;
    std::array<int, 26> trimmedcount = {};

    //If this is the first word, then trimmedword = the word we are looking at
    trimmedword = word.word;

    //Try to add it to the end
    if (baseword.word.length() != 0) {
        overlap = AddWordOverlap(baseword.word, word.word);
        if (overlap == word.word.length()) return;

        trimmedword = (word.word).substr(overlap, (word.word).length() - overlap);

        trimmedcount.fill(0);
    }

    CountLetters(trimmedword, trimmedcount);

    // Try to add the word to the end
    if (CanAddWord(remaining_letters, trimmedcount)) {
        std::string w;
        std::vector<int> b{};
        std::array<int, 26> l{};

        if (append) {
            w = baseword.word + trimmedword;
        } else {
            w = trimmedword + baseword.word;
        }

        CountLetters(w, l);

        auto r = SubtractLetters(gamecounts, l);
        auto q = struct_word{w, -1, l, b};
        ScoreAllWords(q);

        AddWordToHeap(bestwords, q, n_candidates, mintileefficiency);
    }
}

std::vector<struct_word>
BestAddition(struct_word baseword, const std::array<int, 26> &remaining_letters, int n_candidates, float mintileefficiency) {
    std::vector<struct_word> bestwords;

    for (auto word = begin(wordinfo_points); word != end(wordinfo_points); ++word) {
        AddWord(baseword, *word, remaining_letters, bestwords, n_candidates, mintileefficiency, true);
        AddWord(*word, baseword, remaining_letters, bestwords, n_candidates, mintileefficiency, true);
    }
    return bestwords;
}

void PrintWord(struct_word w) {
    for (int i = 0; i < w.word.length(); i++) {
        auto it = std::find(w.blankpositions.begin(), w.blankpositions.end(), i);

        if (it != w.blankpositions.end()) {
            std::cout << "|" << w.word[i] << "|";
        } else {
            std::cout << w.word[i];
        }
    }

    std::cout << ", Score: " << w.score << ", Length Efficiency: " << w.lengthefficiency << ", Tile Efficiency "
              << w.tileefficiency << ", Remaining Letters: ";
    for (int i = 0; i < 26; i++) {
        std::cout << SubtractLetters(gamecounts, w.letters)[i] << " ";
    }
    std::cout << std::endl;
}

void PrintWordFull(struct_word w) {
    auto words = ScoreAllWords(w);
    std::cout << "Word: ";
    for (int i = 0; i < w.word.length(); i++) {
        auto it = std::find(w.blankpositions.begin(), w.blankpositions.end(), i);

        if (it != w.blankpositions.end()) {
            std::cout << "|" << w.word[i] << "|";
        } else {
            std::cout << w.word[i];
        }
    }
    std::cout << std::endl;
    std::cout << "Score: " << w.score << std::endl;
    std::cout << "Length Efficiency: " << w.lengthefficiency << std::endl;
    std::cout << "Tile Efficiency " << w.tileefficiency << std::endl;

    std::cout << "Used Letters: ";
    for (int i = 0; i < 26; i++) {
        std::cout << w.letters[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Remaining Letters: ";
    for (int i = 0; i < 26; i++) {
        std::cout << SubtractLetters(gamecounts, w.letters)[i] << " ";
    }
    std::cout << std::endl;


    std::cout << std::endl;
    std::cout << "Words:" << std::endl;
    for (int i = 0; i < words.size(); i++) {
        std::cout << words[i] << std::endl;
    }

}

std::array<int, 26> SubtractLetters(std::array<int, 26> a, std::array<int, 26> b) {
    std::array<int, 26> r{};
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

void CountLetters(std::string s, std::array<int, 26> &count) {
    for (char &c : s) {
        count[(int) c - (int) 'a'] += 1;
    }
}

template<typename Iter>
Iter RemoveDuplicates(Iter begin, Iter end) {
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

std::vector<std::string> ScoreAllWords(struct_word &s) {
    const int letters[26] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};
    int score = 0;
    int n = s.word.length();

    char char_array[n];
    strcpy(char_array, s.word.c_str());

    std::vector<std::string> words;
    std::vector<std::pair<int, int>> usagecost; //cost, position

    for (int i = 0; i < s.word.length(); i++) {
        usagecost.emplace_back(std::pair<int, int>{0, i});
    }

    words.reserve(s.word.length());

    for (int start = 0; start <= s.word.length() - 2; start++) {
        int before_size = words.size();
        dict->WordsStartWith(char_array + start, 0, words);

        for (int i = before_size; i < words.size(); i++) {
            for (int j = 0; j < words[i].length(); j++) {
                usagecost[start + j].first += letters[(int) s.word[start + j] - (int) 'a'];
            }
        }
    }

    words.erase(RemoveDuplicates(words.begin(), words.end()), words.end());

    std::array<int, 26> remainingletters = SubtractLetters(gamecounts, s.letters);
    GetBlankPosition(s.blankpositions, usagecost, s.word, remainingletters);

    for (auto it = words.begin(); it != words.end(); ++it) {
        score += ScoreWord(*it);
    }

    int tilecost = ScoreWord(s.word);
    for (int i = 0; i < s.blankpositions.size(); i++) {
        score -= usagecost[s.blankpositions[i]].first;
        tilecost -= usagecost[s.blankpositions[i]].first;
    }


    s.lengthefficiency = (float) score / (float) s.word.length();
    s.tileefficiency = (float) score / (float) tilecost;

    s.score = score;

    return words;
}

void GetBlankPosition(std::vector<int> &blankposition, std::vector<std::pair<int, int>> &usagecost, std::string word,
                      std::array<int, 26> remainingletters) {
    std::sort(usagecost.begin(), usagecost.end());

    for (int i = 0; i < 26; i++) {
        int j = 0;
        while (remainingletters[i] < 0) {
            if (((int) word[usagecost[j].second] - (int) 'a') == i) {

                blankposition.push_back(usagecost[j].second);
                remainingletters[i]++;
            }
            j++;
        }
    }
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