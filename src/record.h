#ifndef RECORD_H
#define RECORD_H
#include <string.h>
#include "value.h"
#include <fstream>
#include <iostream>
#include <boost/crc.hpp>
#include <stdio.h>
using namespace std;

class Record {

    private:
        string key;
        Value value;
        unsigned int CalculateChecksum(Record);

    public:
        void set_key(string key);
        void set_value(Value value);
        string get_key();
        Value get_value();
        void write(ofstream *out);
        void write(FILE* &out);
        bool read(FILE* &in);
        bool read(ifstream *in);

};

#endif

