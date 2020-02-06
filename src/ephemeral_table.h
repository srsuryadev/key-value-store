#include<stdint.h>
#include <map>
#include <string>

using namespace std;

class EphemeralTable {

    public:

        ~EphemeralTable();

        EphemeralTable getInstance();

        bool set(string key, string value);
        string get(string key);
        string getPrefix(string key);


    private:

        map<string, string> table;
        EphemeralTable();

};