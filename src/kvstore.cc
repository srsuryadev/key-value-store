
#include <string>
#include "ephemeral_table.h"
#include "log_table.h"

using namespace std;

class KeyValueStore {

    public:

        bool set(string key, string value);
        string get(string key);
};
