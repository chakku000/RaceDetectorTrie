#include <iostream>
#include <vector>
#include <map>
#include <set>

enum ACCESS_TYPE{
    READ,WRITE,
};

/**
 * T スレッド識別子の型
 * U ロック識別子の型
 */
template<typename T,typename U>
class Trie{
    public:
        ACCESS_TYPE a;
        T tid;      // スレッド識別子 tid >= 0 でスレッド,tid == -1 で「2つの異なるスレッド」,tid==-2で「スレッドがない」
        std::map<U,Trie> nodes;
        Trie(T tid_,ACCESS_TYPE a_,std::set<U>& locks){}

        // weakness checkは行わない
        // とにかく追加するだけ
        void insert(const std::set<U>& locks,T tid_,ACCESS_TYPE a_){
            Trie *r = this;
            for(const U& lock : locks){
                if(!nodes.count(lock)){ // 対応するロック識別子がない
                    r->nodes[lock] = Trie();
                }
                r = r->nodes[lock];
            }
            // 現在のノードが対応するロック集合を持ったパスの葉
            // r->a , r->tid と a_,tid_をmeet演算すれば良い
            r->a = meet(r->a,a_);
            r->tid = meet(r->tid,tid_);
        }
};


int main(){

}
