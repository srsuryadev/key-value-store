#ifndef KVSTORE_H
#define KVSTORE_H
#include <string>
#include <iostream>
using namespace std;

class KeyValueStore {

    public:

        bool set(string key, string value);
        string get(string key);
};

#endif
