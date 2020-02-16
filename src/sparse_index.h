#ifndef SPARSE_INDEX_H
#define SPARSE_INDEX_H
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class SparseIndex {


    private:
        static const int SPARSENESS;
        map<string, int> sparse_index_;
        int counter;
        std::ofstream out_index_;
        std::ifstream in_index_;
        string id_;
        bool is_write_;

    public:
        SparseIndex(string, bool);
        int GetOffset(string);
        void put(string, int);
        void Read();
        void Write();
        ~SparseIndex();
};

#endif
