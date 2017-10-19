#include <string>
#include <iostream>
#include <fstream>
/**
 * Trie木を渡せばそのTrieのdotファイルを吐くのでGraphvisなどで適宜可視化できる
 */
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
