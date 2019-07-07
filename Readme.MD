# Scrabbulizer
Attempts to find high scoring scrabble words using up the entire tilebag for [The Riddler](https://fivethirtyeight.com/features/whats-your-best-scrabble-string/) due July 17th 2019.

Super dirty C++ so use at your own risk etc.

## Strategy
The strategy is:
- Score all the words, as well as their subwords
- Pick the top N words (base words), recursively find the M words (new words) in the dictionary that will result in the
  highest score for the aggregate word when the base word and the new word are concatenated together. During this
  concatenation we watch out for:
    - Can we actually add this word? Do we have enough letters?
    - Can we take advantage of any overlap between them? For example "hello" and "look" can be concatenated to "hellook"
- When scoring words we find the optimal place to put the blanks, if we need to have a blank on an "h" it's better to
  put it on the "h" with the fewest usages. Note there is currently a bug in this with de-duplication which might result
  in slightly non-optimal blank placement ¯\_(ツ)_/¯.

This is inherently a greedy, recursive method. We pick the highest scoring words, then concatenate onto those the
words that will result in the highest combined word and keep doing this until we run out of letters. This could result
in some short sightednes on the programs part. To mitigate this we do a few things:
  - Pick more words for the first level of recursion. The first word is going to be a monster during which we will use
    up a lot of letters. Picking a ton of "first" words spreads out our shortsightedness.
  - The concept of "tile efficiency". This is the score of a word (including all subwords) divided by the score of the
    word excluding subwords. Basically a measure of how many times each tile is getting repeated. We would ideally like
    to use words with a high tile efficiency so that we don't blow a ton of tiles on a word that has the highest score,
    but wastes tiles. I have arbitrarily set this at 7, I only consider new words if the base word + the new word has a
    tile efficiency greater than 7.

## Data Structures:
The root of this is the Trie, which stores the whole dictionary in a tree structure. This lets us quickly search for
words in the dictionary, but also find words that begin with certain letters. This is crucial for the scoring function
which is where this program spends the most time. If we stored our dictionary in a hash table, scoring all the words +
subwords would be N^2 in aggregated word length. Using a trie takes this down to N Log(M) in aggregate word length (N) and
average sub-word length (M). I stole my Trie implementation from (Dan Vanderkam)[https://github.com/danvk/performance-boggle].

## Usage:
This is trivially parallelized with OpenMP, there is a CMake file but this is simple enough

### Unit tests
This will just run the unit tests I wrote. Some of these are comented out because there are conflicts with global
variables that I am too lazy to fix.
```bash
g++ -std=gnu++11 -o tests test.cpp scrabble.cpp trie.cpp
./tests
```

### Running
This expects the dictionary to be at "/enable1.txt", do that. This also requires openmp and a compiler that supports C++11
```bash
g++ -fopenmp -Ofast -march=native -std=gnu++11 -o scrabbulizer main.cpp scrabble.cpp trie.cpp
./scrabbulizer
```
This will print the best word it has found so far (approximately) as well as statsu about that word. If you cat this
into a file you can see the _actual_ top word with

```bash
watch ./topout.sh
```
Feel free to play around with numlevel and mintileefficiency that are hardcoded into GetTopAdditions. These contro the minimum
cutoff for tile efficiency and will determine how quickly you get to good words vs the eventual "best" word the algorithm finds.

