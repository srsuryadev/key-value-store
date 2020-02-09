#include <math.h>
#include <string>
using namespace std;


class BloomFilter {

    int false_positive;
    int size;
    int hash_count;

    vector<bool> bloom_filter;
    
    int hash(string elem, int num) {

        int hash_code = 0;
        int power_factor = 1;
        for (int i = 0; i < elem.size(); i++) {
            hash_code += (elem[i] - '0') * power_factor * num;
            power_factor *= 2;
        }
        return hash_code;
    }
    
    public:
        BloomFilter(int num_elem) {
            false_positive = 0.1;
            size = - (num_elem * log(false_positive) /
                        log(2)^2;
            hash_count = (size / num_elem) * log(2);
            bloom_filter = vector<bool> filter(size, false);
        }

        void add(string elem) {
            for (int i = 0; i < hash_count; i++) {
                hash_index = hash(elem, i) % size;
                bloom_filter[hash] = true;
            }
        }

        bool check(string elem) {
            for (int i = 0; i < hash_count; i++) {
               hash_code = hash(elem, i) % size;
               if (!bloom_filter[hash_code]) {
                   return false;
               }
            }
            return true;
        }

}