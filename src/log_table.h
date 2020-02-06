#include<stdint.h>
#include <map>
#include <string>

using namespace std;

class LogTable {

    public:

        LogTable();
        ~LogTable();

        void write(EphemeralTable* ephemeralTable);
        LogTable merge(LogTable* logTable);

        void open(); 
    
};