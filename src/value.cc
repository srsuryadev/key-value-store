#include<string>
#include <ctime>

using namespace std;

class Value {

    private:

        string value;
        int timestamp;
        bool deleted;

        static int time = 0;

    public:

        string get_value() {
            return value;
        }

        long get_time() {
            return timestamp;
        }

        bool is_deleted() {
            return deleted;      
        }

        void set_value(string value) {
            this.value = value;
            this.timestamp = time++;
        }

        void mark_deleted() {
            this.deleted = true;
        }
};