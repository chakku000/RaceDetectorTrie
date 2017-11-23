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

    std::cout << "アクセス1" << std::endl;
    std::set<int> a1{1,2};  // V -> E
    std::cout << "hasWeaknessAccess = " << trie.hasWeaknessAccess(a1,WRITE,1) << std::endl;
    std::cout << "hasRace = " << trie.hasRace(a1,WRITE,1) << std::endl;
    trie.deleteStrongerAccess(a1,WRITE,1,std::set<int>());
    trie.insert(a1,1,WRITE);
    TrieViewer::view<int,int>(trie,"out1.dot");

    std::cout << "アクセス2" << std::endl;
    std::set<int> a2{1};    // E -> S
    std::cout << "hasWeaknessAccess = " << trie.hasWeaknessAccess(a2,READ,2) << std::endl;
    std::cout << "hasRace = " << trie.hasRace(a2,READ,2) << std::endl;
    trie.deleteStrongerAccess(a2,READ,2,std::set<int>());
    trie.insert(a2,2,READ);
    TrieViewer::view<int,int>(trie,"out2.dot");

    std::cout << "アクセス3" << std::endl;
    std::set<int> a3{3};
    std::cout << "hasWeaknessAccess = " << trie.hasWeaknessAccess(a3,WRITE,3) << std::endl;
    std::cout << "hasRace = " << trie.hasRace(a3,WRITE,3) << std::endl;
}
