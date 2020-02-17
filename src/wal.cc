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
        //wal->CreateWriteStream();
        //wal->OpenReadStream();
    }
    return wal;
}

//Truncates the existing wal file if existed
void WAL::CreateWriteStream() {
     _mutex.lock();
    wal_out = fopen(this->file_name.c_str(), "w");
     _mutex.unlock();
}

void WAL::OpenReadStream() {
    wal_in.open(this->file_name, ifstream::in);
}

void WAL::CloseWriteStream() {
    fclose(wal_out);
}

void WAL::CloseReadStream() {
    wal_in.close();
}

bool WAL::Discard() {
    //To be thread safe
    _mutex.lock();
    if (remove(this->file_name.c_str())) {
        return true;
    }
     _mutex.unlock();
    return false;
}

void WAL::Append(Record record) {
    //To be thread safe
    _mutex.lock();
    record.write(wal_out);
    _mutex.unlock();
    fsync(fileno(wal_out));
}

WAL::Iterator::Iterator(WAL *_wal) {
    wal = _wal;
    next_record = 0;
}

bool WAL::Iterator::HasNext() {
    //false if file not open
    if(!wal->wal_in.is_open())
        return false;

    if(next_record != 0)
        return true;

    next_record = new Record;
    if(!next_record->read(&wal->wal_in)) {
        //If false because of checksum match, continue to read next records.
        return false;
    }
    
    return true;
}

Record WAL::Iterator::Next() {
    Record record = *next_record;
    delete next_record;
    next_record = 0;
    return record;
}
