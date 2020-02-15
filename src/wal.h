#ifndef WAL_H
#define WAL_H
#include "record.h"
#include <stdio.h>
#include <mutex>

class WAL {
    private:
        static WAL *wal;
        static mutex _mutex;
        string file_name;
        ofstream wal_out;
        ifstream wal_in;

        WAL();

    public:
        static WAL* GetInstance();
        void CreateWriteStream();
        void OpenReadStream();
        void CloseWriteStream();
        void CloseReadStream();
        bool Discard();
        void Append(Record record);

        class Iterator {
            private:
                WAL *wal;
                Record *next_record;

            public:
                Iterator(WAL*);
                bool HasNext();
                Record Next();
        };

};

#endif
