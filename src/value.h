#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

class Value {

    private:
        string value;
        int timestamp;
        bool deleted;

    public:

        string get_value();
        long get_time();
        bool is_deleted();
        void set_value(string value);
        void mark_deleted();
        //Not Required. Can be removed
        void write(ofstream out);
        //Not Required. Can be removed
        void read(ifstream in);
};

#endif
