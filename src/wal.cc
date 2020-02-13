#include "wal.h"
#include "helper.cc"

WAL::WAL() {
    file_name = "log";
}

static WAL* WAL::GetInstance() {
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
    if (remove(this->file_name)) {
        return true;
    }
    return false;
}

void WAL::Append(Record record) {
    record.write(wal_out);
}