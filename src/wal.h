#ifndef WAL_H
#define WAL_H
#include "record.h"
#include <unistd.h>
#include <stdio.h>
#include <mutex>

class WAL {
    private:
        static WAL *wal;
        static mutex _mutex;
        string file_name;
        FILE* wal_out;
        FILE* wal_in;

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
                size_t file_size;

            public:
                Iterator(WAL*);
                bool HasNext();
                Record Next();
        };

};

#endif

