#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <string>
#include <fstream>
#include <string>

#ifndef INCLUDE_TRIE
#define INCLUDE_TRIE
#endif

enum ACCESS_TYPE{
    READ,WRITE,
};
bool isWeak(ACCESS_TYPE p,ACCESS_TYPE q)
{
    return (p == q) || (p == WRITE);
}

std::ostream& operator<<(std::ostream& os , ACCESS_TYPE a){
    if(a == READ) os << "READ";
    else os << "WRITE";
    return os;
}

int cid = 0;

/**
 * T スレッド識別子の型
 * L ロック識別子の型
 *
 * 制約
 * T は負の値を取れる整数である必要がある
 */
template<typename T,typename L>
class Trie{
    private:
        ACCESS_TYPE meet(ACCESS_TYPE ai,ACCESS_TYPE aj){
            if(ai == aj) return ai;
            else return WRITE;
        }
        T meet(T ti,T tj){
            if(ti == tj){
                return ti;
            }
            else if(ti == -2 or tj == -2){
                assert(ti != -2 or tj != -2);
                return (ti == -2) ? tj : ti;
            }
            else if(ti != tj){
                return -1;
            }
            assert(false);
        }

        /**
         * if p is weaker than q then true
         *                       else false
         */
        bool isWeak(T p,T q)
        {
            return (p == q) || (p == -2);
        }

    public:
        int id;
        ACCESS_TYPE a;
        T tid;      // スレッド識別子 tid >= 0 でスレッド,tid == -1 で「2つの異なるスレッド」,tid==-2で「スレッドがない」
        std::map<L,Trie> nodes;
        /**
         * Constructor
         */
        Trie() : tid(-2) , a(READ) , id(cid++){}

        /**
         * Destructor
         */
        ~Trie(){}

        /**
         * Trie木にアクセスを挿入
         * @detail weakness checkは行わずにとにかく挿入する
         */
        void insert(const std::set<L>& locks,T tid_,ACCESS_TYPE a_){
            Trie *r = this;
            for(const L& lock : locks){
                if(!r->nodes.count(lock)){ // 対応するロック識別子がない
                    r->nodes[lock] = Trie();
                }
                r = &r->nodes[lock];
            }
            // 現在のノードが対応するロック集合を持ったパスの葉
            // r->a , r->tid と a_,tid_をmeet演算すれば良い
            r->a = meet(r->a,a_);
            r->tid = meet(r->tid,tid_);
        }

        /**
         * より弱いアクセスが存在するかのチェック
         * 新しいアクセスより弱いものがあったときにtrueになる
         */
        bool hasWeaknessAccess(const std::set<L> locks,ACCESS_TYPE new_a_type,T new_tid){
            if(isWeak(new_tid,tid) and isWeak(new_a_type,a)){ // 新しいアクセスが該当ノードに対応する過去アクセスよりも弱い
                //std::cerr << "Return True : " << tid << " " << a << std::endl;
                return true;
            }

            for(auto node : nodes){
                // あるノードの子孫に相当するアクセスが持つロックが新しいアクセスに含まれない --> その子孫はweaknessではない
                if(!locks.count(node.first)) continue;

                // もし子ノードから辿ってweakerなアクセスが存在したらtrueを返す
                if(node.second.hasWeaknessAccess(locks,new_a_type,new_tid)) return true;
            }
            //std::cerr << "Return False " << tid << " " << ((a == WRITE) ? "WRITE" : "READ") << std::endl;
            //std::cerr << "New Access = " << new_tid << " " << new_a_type << std::endl;
            //std::cerr << "Weaker = " << std::boolalpha << isWeak(tid,new_tid) << " " << isWeak(a,new_a_type) << std::endl;
            return false;
        }


        void debug(){
            std::cerr << "tid = " << tid << std::endl;
            std::cerr << "type= " << ( (a==READ)?"READ":"WRITE" ) << std::endl;
            std::cerr << "child : ";
            for(auto p : nodes){
                std::cerr << p.first << ", ";
            }
            std::cerr << std::endl;
        }
};


/*
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
*/
