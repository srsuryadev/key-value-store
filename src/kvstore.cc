#include "kvstore.h"

bool KeyValueStore::set(string key, string value) {
    cout<<"Received SET request --- Key: "<<key<<" Value: "<<value<<endl;
    return true;
}

string KeyValueStore::get(string key) {
    cout<<"Received GET request --- Key: "<<key<<endl;
    return "Sample Value";
}
