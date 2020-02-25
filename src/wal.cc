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
    wal_in = fopen(this->file_name.c_str(), "r");
}

void WAL::CloseWriteStream() {
    fclose(wal_out);
}

void WAL::CloseReadStream() {
    fclose(wal_in);
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
    counter = 0;
    T = 0;
    FILE *p_file = NULL;
    p_file = fopen(wal->file_name.c_str(), "r");
    struct stat info;
    stat(wal->file_name.c_str(), &info);
    file_size = info.st_size;
    cout<<"File size: "<<file_size<<endl;
    fclose(p_file);
}

bool WAL::Iterator::HasNext() {
    //false if file not open
    if(wal->wal_in == NULL) {
        cout<<"File opening failed\n";
        return false;
    }

    if(T == file_size) {
        cout<<"EOF\n";
        return false;
    }

    if(next_record != 0)
        return true;
    //cout<<"Gonna read record\n";
    next_record = new Record;
    if(!next_record->read(wal->wal_in)) {
        //If false because of checksum match, continue to read next records.
        return false;
    }
    T += sizeof(*next_record);
    return true;
}

Record WAL::Iterator::Next() {
    Record record = *next_record;
    delete next_record;
    next_record = 0;
    counter++;
    if(counter % 10000 == 0)
        cout<<"Sending "<<counter<<endl;
    return record;
}

