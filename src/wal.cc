#include "wal.h"

WAL* WAL::wal = 0;
mutex WAL::_mutex;

WAL::WAL() {
    file_name = "log";
}

WAL* WAL::GetInstance() {
    //To be thread safe
    lock_guard<mutex> lock(WAL::_mutex);
    if (!wal) {
        wal = new WAL;
    }
    return wal;
}

void WAL::Create() {
    wal_out.open(this->file_name, ofstream::out);
}

void WAL::Close() {
    wal_out.close();
}

bool WAL::Discard() {
    if (remove(this->file_name.c_str())) {
        return true;
    }
    return false;
}

void WAL::Append(Record record) {
    record.write(&wal_out);
}
