//Written by dan vanderkam https://github.com/danvk/performance-boggle
#include "trie.h"

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <utility>
#include <map>
#include <vector>

#include <iostream>

Trie::Trie() : bits_(0) {}

int NumChildren(const SimpleTrie& t) {
    int num_children = 0;
    for (int i=0; i<26; i++) {
        if (t.StartsWord(i)) num_children += 1;
    }
    return num_children;
}

// Memory model: The memory used by an entire Trie is owned by the root node of
// that Trie. Since it would be expensive for each Trie node to remember
// whether it's the root, we maintain a list of all root nodes and the memory
// allocated for them.
std::map<Trie*, char*> root_tries;

// This is messy -- use placement new to get a Trie of the desired size.
Trie* AllocatePT(const SimpleTrie& t, void* where, int* bytes_used) {
    int mem_size = sizeof(Trie) +
                   ((t.IsWord() ? 1 : 0) + ::NumChildren(t)) * sizeof(Trie*);
    *bytes_used += mem_size;
    return new(where) Trie;
}

// Allocate in BFS order to minimize parent/child spacing in memory.
struct WorkItem {
    const SimpleTrie& t;
    Trie* pt;
    int depth;
    WorkItem(const SimpleTrie& tr,
             Trie* ptr, int d) : t(tr), pt(ptr), depth(d) {}
};
Trie* Trie::CompactTrie(const SimpleTrie& t) {
    int alloced = 10 << 20;
    char* root_mem = (char*)malloc(256);  // TODO: alloc the right amount.
    char* raw_bytes = new char[alloced];  // TODO: figure out how much we need.
    int bytes_used = 0;

    std::queue<WorkItem> todo;
    Trie* root = AllocatePT(t, root_mem, &bytes_used);
    bytes_used = 0;
    root_tries.insert(std::make_pair(root, raw_bytes));
    todo.push(WorkItem(t, root, 1));

    while (!todo.empty()) {
        WorkItem cur = todo.front();
        todo.pop();

        // Construct the Trie in the Trie
        const SimpleTrie& t = cur.t;
        Trie* pt = cur.pt;
        pt->SetIsWord(t.IsWord());
        if (t.IsWord()) pt->Mark(0);
        int num_written = 0;
        int off = t.IsWord() ? 1 : 0;
        for (int i=0; i<kNumLetters; i++) {
            if (t.StartsWord(i)) {
                pt->bits_ |= (1 << i);
                pt->data_[off + num_written] =
                        (uintptr_t)AllocatePT(*t.Descend(i), raw_bytes + bytes_used, &bytes_used);
                todo.push(WorkItem(*t.Descend(i), pt->Descend(i), cur.depth + 1));
                num_written += 1;
                if (bytes_used > alloced) {
                    fprintf(stderr, "Used too much memory!");
                    return NULL;
                }
            }
        }
    }
    return root;
}

// Free memory associated with this Trie, if it owns any memory.
void Trie::Delete() {
    this->~Trie();  // free the memory owned by this Trie.
    free(this);     // free the memory used by this node.
}
Trie::~Trie() {
    std::map<Trie*, char*>::iterator it = root_tries.find(this);
    if (it != root_tries.end()) {
        delete[] it->second;
        root_tries.erase(it);
    }
}

// Utility routines that operate on the root Trie node.
bool Trie::IsWord(const char* wd) const {
    if (!wd) return false;
    if (!*wd) return IsWord(); //if we are out of room in the word, return whether we are at a leaf

    int c = *wd - 'a';
    if (c<0 || c>=kNumLetters) return false;

    if (StartsWord(c)) { //if we have somewhere to descend to, descend there
        return Descend(c)->IsWord(wd+1);
    }
    return false; //otherwise return false
}

void Trie::WordsStartWith(const char* wd, int pos, std::vector<std::pair<std::string, bool>>& words) const {
//    std::cout<<wd<<" "<<IsWord()<<std::endl;
//    *wordendsat=IsWord();

//std::string first(name, name + 5);
    if (IsWord()){
        std::string foundword(wd-pos, pos);
        words.push_back(std::make_pair(foundword, true));
    }

    int c = *wd - 'a';
    if (c<0 || c>=kNumLetters) return;

    if (StartsWord(c)) { //if we have somewhere to descend to, descend there
        Descend(c)->WordsStartWith(wd+1, pos+1, words);
    }
//    *wordendsat=true;
}

void Trie::WordsStartWith_pos(const char* wd, int pos, std::vector<int>& wordends) const {

    if (IsWord()){
        wordends.push_back(pos);
    }

    int c = *wd - 'a';
    if (c<0 || c>=kNumLetters) return;

    if (StartsWord(c)) { //if we have somewhere to descend to, descend there
        Descend(c)->WordsStartWith_pos(wd+1, pos+1, wordends);
    }
//    *wordendsat=true;
}

size_t Trie::MemoryUsage() const {
    size_t size = sizeof(*this);
    if (IsWord()) size += sizeof(unsigned);
    size += sizeof(Trie*) * NumChildren();
    for (int i = 0; i < 26; i++) {
        if (StartsWord(i))
            size += Descend(i)->MemoryUsage();
    }
    return size;
}

//void Trie::PrintTrie(std::string prefix) const {
//  if (IsWord()) printf("+"); else printf("-");
//  printf("(%08X) ", bits_);
//  printf("%s\n", prefix.c_str());
//  for (int i=0; i<26; i++) {
//    if (StartsWord(i))
//      Descend(i)->PrintTrie(prefix + std::string(1, 'a' + i));
//  }
//}

Trie* Trie::CreateFromFile(const char* filename) {
    SimpleTrie* t = new SimpleTrie;
    char line[80];
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Couldn't open %s\n", filename);
        delete t;
        return NULL;
    }

    while (!feof(f) && fscanf(f, "%s", line)) {
        t->AddWord(line);
    }
    fclose(f);

    Trie* pt = Trie::CompactTrie(*t);
    delete t;
    return pt;
}


// Plain-vanilla Trie code
inline int idx(char x) { return x - 'a'; }

SimpleTrie* SimpleTrie::AddWord(const char* wd) {
    if (!wd) return NULL;
    if (!*wd) {
        SetIsWord();
        return this;
    }
    int c = idx(*wd);
    if (!StartsWord(c))
        children_[c] = new SimpleTrie;
    return Descend(c)->AddWord(wd+1);
}

SimpleTrie::~SimpleTrie() {
    for (int i=0; i<26; i++) {
        if (children_[i]) delete children_[i];
    }
}

// Initially, this node is empty
SimpleTrie::SimpleTrie() {
    for (int i=0; i<kNumLetters; i++)
        children_[i] = NULL;
    is_word_ = false;
    mark_ = 0;
}