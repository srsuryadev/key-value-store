#include "log_table.h"

const string LogTable::file_name_ = "log_table";
const std::filesystem::path path = "data/";

LogTable::LogTable() {
    auto start = chrono::steady_clock::now();
    cout<<start<<endl;
    this->Create();
}

LogTable::LogTable(string file_name) {
    this->Open(file_name);
}

void LogTable::write(list<Record> &records) {
    std::list<Record>::iterator it;
    for (it = records->begin(); it != records->end(); it++) {
        it->write(this->out);
    }   
}

void LogTable::write(Record record) {
    record.write(this->out);
}

static list<LogTable>& LogTable::GetLogTables() {
    list<LogTable> log_tables;
    boost::filesystem::directory_iterator end;
    for (boost::filesystem::directory_iterator it(data_path_); it != end; ++it) {         
        LogTable log_table(itr->path().filename());
        log_tables.push_back(log_table);
    }
    return log_tables;
}


Record LogTable::Search(string key) {

    LogTable::Iterator *it = new Iterator(this);
    while (it->HasNext()) {
        Record record = it->Next();
        if (record.get_key().compare(key)) {
            return record;
        }
    }
    return NULL;
}

list<Record>& LogTable::SearchPrefix(string key) {
    list<Record> records;
    LogTable::Iterator *it = new Iterator(this);
    while (it->HasNext()) {
        Record record = it->Next();
        if (record.get_key().find(key) == 0) {
            records.push_back(record);
        }
    }
    return records;
}


LogTable& LogTable::Merge(LogTable &log_table1, LogTable &log_table2) { 
    
    LogTable merged_table;
    
    LogTable::Iterator *it1 = new Iterator(&log_table1);
    LogTable::Iterator *it2 = new Iterator(&log_table2);

    Record record1, record2;
    if (it1->HasNext())
        record1 = it1->Next();
    if (it2->HasNext())
        record2 = it2->Next();

   do {

        while (record1.is_deleted() && it1->HasNext())
            record1 = it1->Next();

        while (record2.is_deleted() && it2->HasNext())
            record2 = it2->Next();
        
        if (record1.get_key()
                    .compare(record2.get_key()) < 0) {
            merged_table.write(record1);
            record1 = it1->Next();
        } else if (record1.get_key()
                    .compare(record2.get_key()) > 0) {
            merged_table.write(record2);
            record2 = it2->Next();
        } else {
            if (record1.get_value().get_time() > record2.get_value().get_time()) {
                merged_table.write(record1);
            } else {
                merged_table.write(record2);
            }
            record1 = it1->Next();
            record2 = it2->Next();
        }
    }  while (it1->HasNext() && it2->HasNext());

    do {
        record1 = it1->Next();
        merged_table.write(record1);
    } while (it1->HasNext());

    do {
        record2 = it2->Next();
        merged_table.write(record2);
    } while (it1->HasNext());

    return merged_table;

}



static list<Record>& LogTable::Get(string key) {
    list<Record> records;
    list<LogTable> log_tables = LogTable::GetLogTables();

    std::list<LogTable>::iterator it;
    for (it = log_tables->begin(); it != log_tables->end(); it++) {
        // TODO: Use bloom filter to skip the log tables

        // TODO: if present, seek to the nearest offset using the sparse index
        Record record = it->search(key);
        if (record) {
            records.push_back(record);
        }
    }

    return records;
}

static list<Record>& LogTable::GetPrefix(string key) {
    list<Record> records;
    list<LogTable> log_tables = LogTable::GetLogTables();

    std::list<LogTable>::iterator it;
    for (it = log_tables->begin(); it != log_tables->end(); it++) {
        // TODO: Use bloom filter to skip the log tables

        // TODO: if present, seek to the nearest offset using the sparse index
        list<Record> record_tmp = it->search_prefix(key);
        if (record_tmp && record_tmp.size() > 0) {
            records.insert(this->records.end(), this->record_tmp.begin(),
                                     this->record_tmp.end());
        }
    }
    return records;
}