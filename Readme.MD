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
subwords would be N^2 in aggregated word length. Using a (trie)[https://en.wikipedia.org/wiki/Trie] takes this down to N Log(M) in aggregate word length (N) and
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
> g++ -fopenmp -Ofast -march=native -std=gnu++11 -o scrabbulizer main.cpp scrabble.cpp trie.cpp
> ./scrabbulizer
```
This will print the best word it has found so far (approximately) as well as statsu about that word. If you cat this
into a file you can see the _actual_ top word with. Here a negative in the letter counts indicate that a blank is used
additionally a blank in the printed word is indicated by "|" on either side of the letter

This means "ri|d|dler" means that a blank is used on the first d.

```bash
> watch ./topout.sh
```
Feel free to play around with numlevel and mintileefficiency that are hardcoded into GetTopAdditions. These contro the minimum
cutoff for tile efficiency and will determine how quickly you get to good words vs the eventual "best" word the algorithm finds.

### Reporting
If you want to know about a specific word, then use explainword.cpp
```bash
> g++ -std=gnu++11 -o explainword explainword.cpp scrabble.cpp trie.cpp
> ./explainword myword
Word: myword
Score: 22
Length Efficiency: 3.66667
Tile Efficiency 1.46667
Used Letters: 0 0 0 1 0 0 0 0 0 0 0 0 1 0 1 0 0 1 0 0 0 0 1 0 1 0
Remaining Letters: 9 2 2 3 12 2 3 2 9 1 1 4 1 6 7 2 1 5 4 6 4 2 1 1 1 1

Words:
my
wo
word
or
```
My current "best" word is

<details><summary>recontextualizedhe|m|idemisemiquavers|z|oogeographicallyredeveloperstowawaysunjointingriffinkabobattunad</summary>
<p>
<strong>Word:</strong> recontextualizedhe|m|idemisemiquavers|z|oogeographicallyredeveloperstowawaysunjointingriffinkabobattunad
</p>
<p>
<strong>Score:</strong> 1505
</p>
<p>
<strong>Length Efficiency:</strong> 15.05
</p>
<p>
<strong>Tile Efficiency:</strong> 8.00532
</p>
<p>
<strong>Used Letters:</strong> 9 2 2 4 12 2 3 2 9 1 1 4 3 6 8 2 1 6 4 6 4 2 2 1 2 2
</p>
<p>
<strong>Remaining Letters:</strong> 0 0 0 0 0 0 0 0 0 0 0 0 -1 0 0 0 0 0 0 0 0 0 0 0 0 -1
</p>

#### Words:
re, rec, recon, recontextualize, recontextualized, con, conte, context, contextual, contextualize, contextualized, on, text, textual, ex, al, li, zed, ed, edh, he, hem, hemidemisemiquaver, hemidemisemiquavers, em, mi, mid, id, idem, de, demise, demisemiquaver, demisemiquavers, mis, mise, is, semi, semiquaver, semiquavers, qua, quaver, quavers, ave, aver, avers, er, ers, zoo, zoogeographic, zoogeographical, zoogeographically, geographic, geographical, geographically, graph, graphic, graphical, graphically, rap, phi, hi, hic, call, all, ally, lyre, red, rede, redevelop, redeveloper, redevelopers, dev, devel, develop, develope, developer, developers, eve, el, elope, eloper, elopers, lo, lop, lope, loper, lopers, op, ope, pe, per, erst, stow, stowaway, stowaways, to, tow, towaway, towaways, ow, waw, aw, awa, away, way, ways, ay, ays, sun, un, unjoint, unjointing, jo, join, joint, jointing, in, inti, ti, tin, ting, griff, griffin, rif, riff, if, iff, fin, fink, ink, ka, kab, kabob, ab, abo, bo, bob, ba, bat, batt, battu, at, att, tun, tuna, na, ad
</p>
</details>