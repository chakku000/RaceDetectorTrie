#include <set>
#include <iostream>

#ifndef INCLUDE_TRIE
#include "Trie.hpp"
#endif

#ifndef INCLUDE_VIEW
#include "View.hpp"
#endif

int main(){
    Trie<int,int> trie;

    std::set<int> st1{1,2,3,4};
    std::set<int> st2{1,2,5};
    std::set<int> st3{1,2,3};
    trie.insert(st1,1,WRITE);
    trie.insert(st2,2,WRITE);
    trie.insert(st3,3,WRITE);

    TrieViewer::view<int,int>(trie,"out.dot");
    TrieViewer::view<int,int>(trie,"out2.dot");

    std::set<int> new_access{1,2,3,4};

    bool f = trie.hasWeaknessAccess(new_access,WRITE,3);

    std::cout << std::boolalpha << f << std::endl;
}
