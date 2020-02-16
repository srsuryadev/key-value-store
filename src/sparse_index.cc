#include "sparse_index.h"

const int SparseIndex::SPARSENESS = 2;

SparseIndex::SparseIndex(string id, bool is_write) {
    this->id_ = id;
    this->is_write_ = is_write;
    this->counter = 0;

    if (this->is_write_) {
        out_index_.open(id, std::ofstream::out);
    } else {
        in_index_.open(id, std::ifstream::in);
    }
}

int SparseIndex::GetOffset(string key) {
    return sparse_index_.lower_bound(key)->second;
}

void SparseIndex::put(string key, int offset) {
    if(counter % SPARSENESS == 0) {
        cout<<"Sparse Index created at this point --- "<<key<<" "<<offset<<endl;
        sparse_index_[key] = offset;
    }
    counter++;
}

void SparseIndex::Read() {
    //Reading only the map
    struct kv {
        string k;
        int v;
    };
    struct kv T;
    while(in_index_.read((char *) &T, sizeof(T))) {
        this->sparse_index_[T.k] = T.v;
    }
}

void SparseIndex::Write() {
    //Writing only the map
    for(auto it = sparse_index_.begin(); it!=sparse_index_.end(); it++) {
        struct kv {
            string k;
            int v;
        };
        struct kv T;
        T.k = it->first;
        T.v = it->second;
        out_index_.write((char *) &T, sizeof(T));
    }
    out_index_.flush();
}

SparseIndex::~SparseIndex() {
    if (this->is_write_) {
        out_index_.close();
    } else {
        in_index_.close();
    }
}
