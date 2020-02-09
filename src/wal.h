#include "record.h"
#include <stdio.h>

class WAL {

    static WAL *wal;
    string file_name;
    ofstream wal_out;

    WAL();

    Record read(){
        Record record;
        return record->read(wal_out);
    }

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
                    return wal->read();
                }
        }

};