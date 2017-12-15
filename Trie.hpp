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
        ACCESS_TYPE a;
        T tid;      // スレッド識別子 tid >= 0 でスレッド,tid == -1 で「2つの異なるスレッド」,tid==-2で「スレッドがない」
        std::map<L,Trie> nodes;
        /**
         * Constructor
         */
        Trie() : tid(-2) , a(READ){}

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
            std::cout << tid << " " << a << std::endl;
            if(isWeak(tid,new_tid) and isWeak(a,new_a_type)){ // 新しいアクセスが該当ノードに対応する過去アクセスよりも弱い
                std::cout << "WEAKER" << std::endl;
                std::cout << "NODE " << tid << " " << a << std::endl;
                std::cout << "NOEW " << new_tid << " " << new_a_type << std::endl;
                return true;
            }

            for(auto node : nodes){
                // あるノードの子孫に相当するアクセスが持つロックが新しいアクセスに含まれない --> その子孫はweaknessではない
                if(!locks.count(node.first)) continue;

                // もし子ノードから辿ってweakerなアクセスが存在したらtrueを返す
                if(node.second.hasWeaknessAccess(locks,new_a_type,new_tid)) return true;
            }
            return false;
        }

        /**
         * 競合検査
         * @detail 競合がある場合にtrue,無い場合にfalseを返す
         */
        bool hasRace(const std::set<L> locks,ACCESS_TYPE new_a_type,T new_tid)
        {
            for(auto node : nodes){
                if(locks.count(node.first))
                {// Case 1 : ノードnに到達する辺lがe.Lのロックに含まれている場合,共有するロックがあるので競合しない
                    continue;
                }
                else if(meet(new_tid,tid)==-1 and meet(new_a_type,a)==WRITE)
                {// Case 2: Case1でなく,アクセススレッドとノード対応すれどのmeetがt_buttom,meet(e.a,n.a) = writeの場合は競合
                    return true;
                }
                else
                {// Case3 : Case1,2以外の場合は子ノードをrootとする木を探索する必要がある
                    if(node.second.hasRace(locks,new_a_type,new_tid))
                    {   // 子ノードを根とする木で競合がある場合は全体で競合があると判定
                        return true;
                    }
                }
            }
            return false;
        }

        /**
         * ノード削除
         * @detail 新しいアクセスよりも強いアクセスを履歴から削除
         * 愚直実装する.具体的にはこれまで辿ってきたロックの集合を保持していて各ノードでチェック
         * ノードの削除はノードの書き換えで実現するのでFreeを行わない.
         * Freeを行うような実装を刷る場合は,子ノードがtid=-2で,さらに子孫ノードが無い場合に解放すればよい
         */
        void deleteStrongerAccess(const std::set<L>& locks,ACCESS_TYPE new_a_type,T new_tid,std::set<int> traversed)
        {
            // ノードが新しいアクセスよりstrongerかをチェックしてdelete
            if(isWeak(new_a_type,a) and isWeak(new_tid,tid)){
                bool subset = true;   // 新しいアクセスがこれまで辿ってきたロックの部分集合である場合にtrue
                // ロックが部分集合か?
                for(const L& l : locks){
                    if(!traversed.count(l)){
                        subset = false;
                        break;
                    }
                }
                if(subset)
                {// 新しいアクセスのロック集合が辿ってきたロック集合の部分集合の場合,このノードを削除
                    tid = -2;       //該当ノードのスレッドを「t_top = -2 (スレッドがない)」状態にする
                    a = READ;       //該当ノードのアクセスタイプをREADにする
                }
            }

            // 子ノードからdelete
            for(auto node : nodes){
                traversed.insert(node.first);
                deleteStrongerAccess(locks,new_a_type,new_tid,traversed);
                traversed.erase(node.first);
            }
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

