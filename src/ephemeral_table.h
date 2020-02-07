#include<stdint.h>
#include <map>
#include <string>

using namespace std;

class EphemeralTable {

    public:

        ~EphemeralTable();

        EphemeralTable get_instance();

        bool set(string key, string value);
        string get(string key);
        string get_prefix(string key);


    private:

        map<string, string> table;
        EphemeralTable();

};