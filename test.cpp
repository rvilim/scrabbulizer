#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.h"
#include "scrabble.h"


std::array<int, 26> gamecounts;
Trie *dict;
std::vector<struct_word> wordinfo_points;

TEST_CASE( "Score a word", "[ScoreWord]" ) {
    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/enable1.txt";
    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
    dict = Trie::CreateFromFile(dictionary_path);

    ScoreDictionary(dictionary_path);

    std::string a = "hello";
    REQUIRE( ScoreWord(a) == 8 );
    a="zzz";
    REQUIRE( ScoreWord(a) == 30 );
}


TEST_CASE( "Score all words", "[ScoreWord]" ) {
    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/enable1.txt";
    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
    dict = Trie::CreateFromFile(dictionary_path);

    ScoreDictionary(dictionary_path);
    REQUIRE( ScoreAllWords_dedupe("hello") == 5+7+8+2+3+2 ); //he + hell + hello + el + ell
    REQUIRE( ScoreAllWords_dedupe("it") == 2 );
}

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
//
//TEST_CASE( "Best addition after", "[BestAddition]" ) {
//    auto dictionary_path = "/Users/rvilim/repos/scrabbulizer/test/bestaddition1.txt";
//    gamecounts =  {9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1};
//    dict = Trie::CreateFromFile(dictionary_path);
//
//    ScoreDictionary(dictionary_path);
//
//    std::array<int, 26> l = {};
//    CountLetters("hello", l);
//    struct_word baseword =  {
//        "hello",
//        8,
//        l
//    };
//
//    auto r= SubtractLetters(gamecounts, l);
//    auto a = BestAddition(baseword, r);
//}

