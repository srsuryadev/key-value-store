#include<string>
#include <ctime>
#include <iostream>
#include <fstream>
using namespace std;

class Value {

    private:
        string value;
        int timestamp;
        bool deleted;
        static int time;

    public:

        string get_value();
        long get_time();
        bool is_deleted();
        void set_value(string value);
        void mark_deleted();
        void write(ofstream out);
        void read(ifstream in);
};