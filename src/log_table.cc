#include "log_table.h"

const string LogTable::file_name_ = "logtable";
const boost::filesystem::path LogTable::data_path_ = "/data";


LogTable::LogTable() {
    this->Create();
}

LogTable::LogTable(string file_name) {
    this->Open(file_name);
}

LogTable:: ~LogTable() {
    if (!this->in_logtable_.is_open())
        this->CloseIn();
    if (!this->out_logtable_.is_open())
        this->CloseOut();
}

void LogTable::Write(list<Record> &records) {
    std::list<Record>::iterator it;
    for (it = records.begin(); it != records.end(); it++) {
        it->write(&this->out_logtable_);
    }   
}

void LogTable::Write(Record &record) {
    record.write(&this->out_logtable_);
}


Record* LogTable::Search(string key) {

    LogTable::Iterator *it = new Iterator(this);
    while (it->HasNext()) {
        Record* record = it->Next();
        if (record->get_key().compare(key)) {
            return record;
        }
    }
    return NULL;
}

list<Record>* LogTable::SearchPrefix(string key) {
    list<Record>* records = new list<Record>();
    LogTable::Iterator *it = new Iterator(this);
    while (it->HasNext()) {
        Record* record = it->Next();
        if (record->get_key().find(key) == 0) {
            records->push_back(*record);
        }
    }
    return records;
}




