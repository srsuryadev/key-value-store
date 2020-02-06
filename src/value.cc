#include<string>

using namespace std;

class Value {

    private:

        string value;
        long timestamp;
        bool deleted;

    public:

        string getValue() {
            return value;
        }

        long getTime() {
            return timestamp;
        }

        bool isDeleted() {
            return deleted;      
        }


};