#include "kvstore.h"

KeyValueStore::KeyValueStore() {
    first_skip_list = new SkipList();
    second_skip_list = new SkipList();
    current_skip_list = first_skip_list;
}

bool KeyValueStore::set(string key, string value) {
    cout<<"Received SET request --- Key: "<<key<<" Value: "<<value<<endl;
    current_skip_list->put(key, value);
    if (++current_skip_list->count == CUT_OFF_COUNT) {
        // TODO: force first list to disk
        // use the other list
        if ((uintptr_t)current_skip_list == (uintptr_t)first_skip_list) {
            current_skip_list = second_skip_list;
        } else {
            current_skip_list = first_skip_list;
        }
    }
    return true;
}

string KeyValueStore::get(string key) {
    cout<<"Received GET request --- Key: "<<key<<endl;
    return current_skip_list->get(key);
}

vector<string> KeyValueStore::get_prefix(string prefix_key) {
    vector<string> ret_val;    
    cout<<"Received GET request for prefix --- Key: "<< prefix_key <<endl;
    return current_skip_list->get_keys_for_prefix(prefix_key);    
}