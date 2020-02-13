#include "record.h"
#include <stdio.h>
#include <mutex>

class WAL {
    private:
        static WAL *wal;
        static mutex _mutex;
        string file_name;
        ofstream wal_out;

        WAL();

        //Why is this needed here?
        /*Record read() {
            Record record;
            record.read(&wal_out);
            return record;
        }*/

    public:
        static WAL* GetInstance();
        void Create();
        void Close();
        bool Discard();
        void Append(Record record);
        class Iterator;

        class Iterator {

            WAL* wal;

            public:
                Iterator(WAL* wal) {
                    this->wal = wal;
                }

                bool HasNext() {
                    //TODO do check for end of file or checksum
                    return true;
                }

                Record Next(){
                    //return wal->read();
                }
        };

};