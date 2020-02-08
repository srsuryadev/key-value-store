#include <string.h>
#include <value.h>
#include <fstream>
#include <iostream>
using namespace std;

class Record {

    private:
        string key;
        Value value;
        unsigned int checksum;

        void CalculateChecksum(); 

    public:
        void set_key(string key);
        void set_value(Value value);
        void write(ofstream out);
        void read(ifstream in);

};