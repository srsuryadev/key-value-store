#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class SparseIndex {


    private:
        map<string, int> sparse_index_;

        std::ofstream out_index_;
        std::ifstream in_index_;

        string id_;
        bool is_write_;

    public:

        SparseIndex(string id, bool is_write) {
            this->id_ = id;
            this->is_write_ = is_write;

            if (this->is_write_) {
                out_index_.open(id, std::ofstream::out);
            } else {
                in_index_.open(id, std::ifstream::in);
            }
        }

        int GetOffset(string key) {
            return sparse_index_.lower_bound(key)->second;
        }

        bool put(string key, int offset) {
            sparse_index_[key] = offset;
        }

        void Read() {
            in_index_.read((char *) this, sizeof(*this));
        }

        void Write() {
            out_index_.write((char *) this, sizeof(*this));
            out_index_.flush();
        }

        ~SparseIndex() {
             if (this->is_write_) {
                out_index_.close();
            } else {
                in_index_.close();
            }
        }

};