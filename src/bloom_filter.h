#include <math.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


class BloomFilter {

    double false_positive;
    int size;
    int hash_count;

    vector<bool> bloom_filter;
    
    std::ofstream out_index_;
    std::ifstream in_index_;

    string id_;
    bool is_write_;

    int hash_func(string elem, int num) {

        int hash_code = 0;
        int power_factor = 1;
        for (int i = 0; i < elem.size(); i++) {
            hash_code += (elem[i] - '0') * power_factor * num;
            power_factor *= 2;
        }
        return hash_code;
    }
    
    public:

        BloomFilter(string id, int num_elem, bool is_write_) {

            this->false_positive = 0.1;
            this->size = - (num_elem * log(false_positive) /
                        pow(log(2.0), 2.0));
            this->hash_count = (size / num_elem) * log(2);
            
            vector<bool> temp(size, false);
            this->bloom_filter = temp;

            this->id_ = id;
            this->is_write_ = is_write_;
            if (this->is_write_) {
                this->out_index_.open(this->id_, std::ofstream::out);
            } else {
                this->in_index_.open(this->id_, std::ifstream::in);
            }
        }

        void add(string elem) {
            for (int i = 0; i < hash_count; i++) {
                int hash_index = hash_func(elem, i) % size;
                bloom_filter[hash_index] = true;
            }
        }

        bool check(string elem) {
            for (int i = 0; i < hash_count; i++) {
               int hash_code = hash_func(elem, i) % size;
               if (!bloom_filter[hash_code]) {
                   return false;
               }
            }
            return true;
        }

        void Read() {

            int size;
            in_index_.read((char*)&size, sizeof(int));
            this->size = size;
            for (int i = 0; i < size; i++) {
                bool value;
                in_index_.read((char*)&value, sizeof(bool));
                this->bloom_filter[i] = value; 
            }
        }

        void Write() {
            cout << "writing bloom data" << endl;
            out_index_.write((char*)&this->size, sizeof(int));
            for (int i = 0; i < this->size; i++) {
                bool value = this->bloom_filter[i];
                out_index_.write((char*)&value, sizeof(bool));
            }
            out_index_.flush();
        }

        ~BloomFilter() {
             if (this->is_write_) {
                out_index_.close();
            } else {
                in_index_.close();
            }
        }

};