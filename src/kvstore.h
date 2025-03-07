#ifndef KVSTORE_H
#define KVSTORE_H
#include <string>
#include <iostream>
#include <vector>
#include "skiplist.h"
#include "log_table.h"
#include "wal.h"
#include "value.h"
#include <mutex>

using namespace std;

// count at which to abandon current skiplist
#define CUT_OFF_COUNT 500000

class KeyValueStore {
    public:
        KeyValueStore();
        bool set(string key, string value);
        string get(string key);
        vector<string> get_prefix(string prefix_key);

    private:

        static mutex _mutex;

        SkipList* current_skip_list;
        SkipList* prev_skip_list;
        SkipList* first_skip_list;
        SkipList* second_skip_list;
};

#endif
