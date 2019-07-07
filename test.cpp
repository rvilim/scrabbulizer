#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define MINTILEEFFICIENCY 6
#include "catch.h"
#include "scrabble.h"

bool CompareByWordScore(const struct_word &a, struct_word &b);
std::array<int, 26> gamecounts;
Trie *dict;
std::vector<struct_word> wordinfo_points;

//TEST_CASE( "Score a word", "[ScoreWord]" ) {
//    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/enable1.txt";
//    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
//    dict = Trie::CreateFromFile(dictionary_path);
//
//    ScoreDictionary(dictionary_path);
//
//    std::string a = "hello";
//    REQUIRE( ScoreWord(a) == 8 );
//    a="zzz";
//    REQUIRE( ScoreWord(a) == 30 );
//}
//
//
//TEST_CASE( "Score all words", "[ScoreWord]" ) {
//    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/enable1.txt";
//    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
//    dict = Trie::CreateFromFile(dictionary_path);
//    std::vector<int> blankpositions;
//
//    ScoreDictionary(dictionary_path);
//
//    std::array<int, 26> l = {0,0,0,0,1,0,0,1,0,0,0,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0};
//    auto a = struct_word {
//        "hello",
//        -1,
//        l,
//        blankpositions
//    };
//    ScoreAllWords(a);
//    REQUIRE( a.score == 5+7+8+2+3+2 ); //he + hell + hello + el + ell + lo
//
//
//    l = {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0};
//    auto b = struct_word {
//        "it",
//        -1,
//        l,
//        blankpositions
//    };
//    ScoreAllWords(b);
//    REQUIRE( b.score == 2 );
//}

TEST_CASE( "Can Add Words", "[CanAddWord]" ) {
    std::array<int, 26> remaining_letters = {0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0};
    std::array<int, 26> word_letters = {0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0};

    REQUIRE(CanAddWord(remaining_letters, word_letters));

    word_letters = {0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0};
    REQUIRE(CanAddWord(remaining_letters, word_letters));

    word_letters = {0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0};
    REQUIRE(!CanAddWord(remaining_letters, word_letters));

    word_letters = {0,0,0,0,0,0,0,0,0,0,3,1,1,0,0,0,0,0,0,0};
    REQUIRE(CanAddWord(remaining_letters, word_letters));

    word_letters = {0,0,0,0,0,0,0,0,0,0,3,1,2,0,0,0,0,0,0,0};
    REQUIRE(!CanAddWord(remaining_letters, word_letters));

    word_letters = {0,0,0,0,0,0,0,0,0,0,3,1,1,1,0,0,0,0,0,0};
    REQUIRE(!CanAddWord(remaining_letters, word_letters));
}

TEST_CASE( "How many overlapping letters", "[AddWordOverlap]" ) {
    std::string baseword = "Hello";
    std::string newword = "llodsf";

    REQUIRE(AddWordOverlap(baseword, newword)==3);

    newword = "ollodsf";
    REQUIRE(AddWordOverlap(baseword, newword)==1);

    newword = "llllo";
    REQUIRE(AddWordOverlap(baseword, newword)==0);

}

TEST_CASE("Blank positions double", "GetBlankPositions") {
    std::string word = "helloll";
    std::array<int, 26> remainingletters={0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::vector<std::pair<int, int>> usagecost={
        std::pair<int, int> {1,0},
        std::pair<int, int> {4,1},
        std::pair<int, int> {4,2},
        std::pair<int, int> {3,3},
        std::pair<int, int> {7,4},
        std::pair<int, int> {2,5},
        std::pair<int, int> {1,6}
    };

    std::vector<int> a;
    GetBlankPosition(a, usagecost, word, remainingletters);

    REQUIRE(((a[0]==6 && a[1]==5) || (a[1]==6 && a[0]==5)));
}

TEST_CASE("Blank positions singles", "GetBlankPositions") {
    std::string word = "helloll";
    std::array<int, 26> remainingletters={0,0,0,0,-1,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::vector<std::pair<int, int>> usagecost={
            std::pair<int, int> {1,0},
            std::pair<int, int> {4,1},
            std::pair<int, int> {4,2},
            std::pair<int, int> {3,3},
            std::pair<int, int> {7,4},
            std::pair<int, int> {2,5},
            std::pair<int, int> {1,6}
    };

    std::vector<int> a;
    GetBlankPosition(a, usagecost, word, remainingletters);
    REQUIRE(((a[0]==1 && a[1]==6) || (a[1]==6 && a[0]==1)));
}

bool CompareByWordScore(const struct_word &a, struct_word &b)
{
    return a.score < b.score;
}

TEST_CASE( "Best addition after", "[BestAddition]" ) {
    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/test/bestaddition1.txt";
    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
    dict = Trie::CreateFromFile(dictionary_path);

    ScoreDictionary(dictionary_path);
    std::string w = "antiferromagnetismspsychopathologically";
    std::array<int, 26> l = {};
    CountLetters(w, l);
    struct_word baseword =  {
        w,
        ScoreWord(w),
        l
    };

    auto r= SubtractLetters(gamecounts, l);
    auto a = BestAddition(baseword, r,5, 0.0);

    std::sort(a.begin(), a.end(), CompareByWordScore);
    std::cout<<a.size()<<std::endl;
    REQUIRE(a[a.size()-1].word=="antiferromagnetismspsychopathologicallyprequalifications");
}

