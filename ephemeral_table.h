#include<stdint.h>
#include <map>
#include <string>


class EphemeralTable {

    public:

        ~EphemeralTable();

        EphemeralTable getInstance();

        bool set(string key, string value);
        string get(string key);
        Iterator* getPrefix(string key);


    private:

        map<string, string> table;
        EphemeralTable();

}