#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <string>
#include <fstream>
#include <string>

enum ACCESS_TYPE{
    READ,WRITE,
};

int cid = 0;

/**
 * T スレッド識別子の型
 * U ロック識別子の型
 *
 * 制約
 * T は負の値を取れる整数である必要がある
 */
template<typename T,typename U>
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
    public:
        int id;
        ACCESS_TYPE a;
        T tid;      // スレッド識別子 tid >= 0 でスレッド,tid == -1 で「2つの異なるスレッド」,tid==-2で「スレッドがない」
        std::map<U,Trie> nodes;
        Trie() : tid(-2) , a(READ) , id(cid++){}
        ~Trie(){
            //std::cout << "delete node " << id << std::endl;
            //std::cout << "\tchild is ";
            //for(auto p : nodes){
            //    std::cout << p.first << " ";
            //}
            //std::cout << std::endl;
        }

        // weakness checkは行わない
        // とにかく追加するだけ
        void insert(const std::set<U>& locks,T tid_,ACCESS_TYPE a_){
            Trie *r = this;
            for(const U& lock : locks){
                std::cout << "looking for lock " << lock << std::endl;
                //if(!nodes.count(lock)){ // 対応するロック識別子がない
                //    std::cout << "no lock" << std::endl;
                //    //r->nodes[lock] = Trie();
                //    //r->nodes[lock];
                //    //r->nodes.insert(std::make_pair(lock,Trie()));
                //}
                // r = &r->nodes[lock]; どっちになるんだ...?
                r = &r->nodes[lock];
            }
            // 現在のノードが対応するロック集合を持ったパスの葉
            // r->a , r->tid と a_,tid_をmeet演算すれば良い
            r->a = meet(r->a,a_);
            r->tid = meet(r->tid,tid_);
            std::cout << "make leaf" << std::endl;
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

namespace TrieViewer{
    int label=0;
    template<typename T,typename U>
    void dfs(Trie<T,U> trie,std::ofstream& file){
        std::string name = std::to_string(label);
        // ノードラベル出力
        file << "\t" << name << "[" << "label=\"";
        std::string atype = (trie.a == READ) ? "READ" : "WRITE";
        file << "(" << trie.tid << " " << atype << ")\",];" << std::endl;

        for(auto child : trie.nodes){
            U lk = child.first;
            Trie<T,U> node = child.second;
            std::string cname = std::to_string(++label);
            file << "\t" << name << " -> " << cname << "[" << "label = \" " << lk << "\"" << "];" << std::endl;
            dfs<T,U>(node,file);
        }
    }

    template<typename T,typename U>
    void view(Trie<T,U> trie,const std::string file_name){
        std::ofstream triefile(file_name);
        if(!triefile.is_open()){
            std::cerr << "ERROR in function view in namespace Viewer. Cannnot open file trie.dot" << std::endl;
            return;
        }
        triefile << "digraph{" << std::endl;
        label = 0;
        dfs<T,U>(trie,triefile);
        triefile << "}" << std::endl;
        triefile.close();
    }
};


int main(){
    Trie<int,int> trie;

    std::set<int> st1{1,2,3,4};
    std::set<int> st2{1,2,5};
    trie.insert(st1,0,WRITE);
    trie.insert(st2,0,WRITE);

    //std::cout << "--------------------" << std::endl;

    //trie.debug();

    //Trie<int,int> t2 = trie.nodes[1];
    //t2.debug();

    //Trie<int,int> t3 = t2.nodes[2];
    //t3.debug();

    TrieViewer::view<int,int>(trie,"out.dot");
}
