#include "sparse_index.h"

const int SparseIndex::SPARSENESS = 1000;

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
    auto it = sparse_index_.upper_bound(key);
    if(it == sparse_index_.begin())
        return 0;
    it--;
    //cout<<"Returning offset: "<<it->second<<endl;
    return it->second;
}

void SparseIndex::put(string key, int offset) {
    if(counter % SPARSENESS == 0) {
        //cout<<"Sparse Index created at this point --- "<<key<<" "<<offset<<endl;
        sparse_index_[key] = offset;
    }
    counter++;
    cout<<"Added to sparse index\n";
}

void SparseIndex::Read() {
    //Reading only the map
    int size = 0;
    int v = 0;
    char k[129];
    while(in_index_.read((char*)&size, sizeof(int))) {
        in_index_.read(k, size);
        k[size] = '\0';
        in_index_.read((char*)&v, sizeof(int));
        this->sparse_index_[string(k)] = v;
    }
}

void SparseIndex::Write() {
    //Writing only the map
    cout << "writing sparse index for " <<this->id_ << endl;
    for(auto it = sparse_index_.begin(); it!=sparse_index_.end(); it++) {
        int size = it->first.length();
        out_index_.write((char *) &size, sizeof(int));
        out_index_.write(it->first.c_str(), size);
        out_index_.write((char*)&(it->second), sizeof(int));        
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
