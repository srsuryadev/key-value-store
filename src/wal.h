#include "record.h"
#include <stdio.h>

class WAL {

    static WAL *wal;
    string file_name;
    ofstream wal_out;

    WAL();

    public:
        static WAL* GetInstance();
        void Create();
        void Close();
        bool Discard();
        void Append(Record record);

};