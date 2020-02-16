#include "log_table.h"

const string LogTable::file_name_ = "logtable";
const boost::filesystem::path LogTable::data_path_ = "data";


LogTable::LogTable() {
    cout<<"created file in write mode"<<endl;
    this->Create();
}

LogTable::LogTable(const LogTable &logtable) {
    this->file_name = logtable.file_name; 
    this->Open(this->file_name);
} 

void LogTable::Discard() {
    if (remove((data_path_.string() + "/" + this->file_name).c_str()) == 0)
        cout << "successfully deleted: " << this->file_name << endl;
    else 
        cout << "unable delete: " << this->file_name << endl;
    

    if (remove((data_path_.string() + "/" + this->file_name + "_bloom").c_str()) == 0)
        cout << "successfully deleted: " << this->file_name + "_bloom"<< endl;
    else 
        cout << "unable delete: " << this->file_name + "_bloom" << endl;
    
}
  

LogTable::LogTable(string file_name) {
    this->file_name = file_name;
    this->Open(this->file_name);
    
}

LogTable:: ~LogTable() {
    if (!this->in_logtable_.is_open())
        this->CloseIn();
    if (!this->out_logtable_.is_open()) {
        this->CloseOut();
    }
}

void LogTable::Write(list<Record> &records) {
    std::list<Record>::iterator it;
    for (it = records.begin(); it != records.end(); it++) {
        this->bloom_filter->add(it->get_key());
        it->write(&this->out_logtable_);
    }   
    cout<< "written data" << endl;
    this->out_logtable_.flush();
    this->bloom_filter->Write();
}

void LogTable::Write(Record &record) {
    record.write(&this->out_logtable_);
    this->bloom_filter->add(record.get_key());
    cout<< "written record" << endl;
    this->out_logtable_.flush();
}


Record* LogTable::Search(string key) {

    LogTable::Iterator *it = new Iterator(this);
    while (it->HasNext()) {
        Record* record = it->Next();
        cout << "Key: " << record->get_key() << endl;
        if (record->get_key().compare(key) == 0) {
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




