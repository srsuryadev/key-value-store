#include <stdint.h>
#include <map>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <list>
#include <cstdlib>
#include <ctime>
#include "bloom_filter.h"
#include "record.h"

using namespace std;

class LogTable {

    private:

        
        static const  string file_name_;
        static const  boost::filesystem::path data_path_;


        std::ofstream out_logtable_;
        std::ifstream in_logtable_;

        string NextFileName() {
            string new_file = file_name_  + "_" + std::to_string(time(0)) + "_" + to_string(clock());
            return new_file;
        }

        void Create() {
            this->file_name = this->NextFileName();
            cout << "great" << endl;
            cout << "created: " << this->file_name << endl;
            string path = data_path_ .string() + "/" + this->file_name;
            this->bloom_filter = new BloomFilter( data_path_ .string() + "/" 
                                                    + "bloom_" + this->file_name , 
                                                        10000, true);
            out_logtable_.open(data_path_ .string() + "/" + this->file_name, 
                                                                    std::ofstream::out);
        }

        void Open(string file_name) {
            this->file_name = file_name;
            this->bloom_filter = new BloomFilter( data_path_ .string() + "/" 
                                                    + "bloom_"+ this->file_name , 
                                                    10000, false);
            cout << "read bloom" <<endl;
            this->bloom_filter->Read();
            in_logtable_.open(data_path_ .string() + "/"  + file_name, 
                                                                std::ifstream::in);
        }

        Record Read() {
            Record record;
            record.read(&this->in_logtable_);
            return record;
        }

        void CloseOut() {
            out_logtable_.close();
        }

        void CloseIn() {
            in_logtable_.close();
        }


    public:

        BloomFilter* bloom_filter;
       
        string file_name;

        LogTable();
        LogTable(const LogTable &logtable);

        ~LogTable();

        LogTable(string file_name);

        void Write(list<Record> &records);
        void Write(Record &record);
        void Discard();

        Record* Search(string key);
        list<Record>* SearchPrefix(string key);


        class Iterator {

            LogTable* logtable;
            Record *next_record;

            public:
                Iterator(LogTable* logtable) {
                    this->logtable = logtable;
                }

                bool HasNext() {
                    if (!logtable->in_logtable_.is_open()) {
                       next_record = 0; 
                       return false;
                    }

                    next_record = new Record;
                    if (!next_record->read(&logtable->in_logtable_)) {
                        next_record = 0;
                        return false;
                    }
                    //TODO do check for end of file or checksum
                    return true;
                }


                Record* Move() {
                    if (HasNext())
                        return next_record;
                    return 0;
                }
                Record* Next(){
                    return next_record;
                }
        };



        static Record* Get(string key) {
            Record* record = 0;
            list<LogTable>* log_tables = LogTable::GetLogTables();

            std::list<LogTable>::iterator it;
            for (it = log_tables->begin(); it != log_tables->end(); it++) {
                // TODO: Use bloom filter to skip the log tables
                cout << "Going to read and check bloom" << endl;
                if (!it->bloom_filter->check(key)) {
                    cout << "Not present in bloom " << it->file_name << endl;
                    continue;
                } else {
                    cout << "Key is present in bloom" << endl;
                }

                // TODO: if present, seek to the nearest offset using the sparse index
                Record* temp_record = it->Search(key);
                if (temp_record && !temp_record->get_value().is_deleted()) {
                    if (record) {
                        if (record->get_value().get_time() <
                                temp_record->get_value().get_time()) {
                            record = temp_record;
                        }
                    } else {
                        record = temp_record;
                    }
                }
            }

            return record;
        }

        static list<Record>* GetPrefix(string key) {
            list<Record>* records = new list<Record>();
            list<LogTable>* log_tables = LogTable::GetLogTables();

            std::list<LogTable>::iterator it;
            for (it = log_tables->begin(); it != log_tables->end(); it++) {
                // TODO: Use bloom filter to skip the log tables

                // TODO: if present, seek to the nearest offset using the sparse index
                list<Record>* record_tmp = it->SearchPrefix(key);
                if (record_tmp->size() > 0) {
                    records->insert(records->end(), record_tmp->begin(),
                                                    record_tmp->end());
                }
            }
            return records;
        }

        
        static list<LogTable>* GetLogTables() {

            list<LogTable>* log_tables = new list<LogTable>();
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir (data_path_.c_str())) != NULL) {
                while ((ent = readdir (dir)) != NULL) {
                    string file_name = ent->d_name;
                    if (file_name.find(file_name_) == 0) {
                        cout << ent->d_name << endl;
                        LogTable log_table(ent->d_name);
                        log_tables->push_back(log_table);
                    }
                }
                closedir (dir);
            } 
            return log_tables;
        }

        static void Merge() {
            
            list<LogTable>* log_tables = LogTable::GetLogTables();
            if (log_tables->size() < 2)
                return;
            
            LogTable *merge_table = new LogTable();
            std::list<LogTable>::iterator it;
            it = log_tables->begin();
            LogTable logtable1 = *it;
            it++;
            
            LogTable logtable2 = *it;
            it++;
           
            LogTable::Merge2(merge_table, logtable1, logtable2);
            logtable1.Discard();
            logtable2.Discard();

            string file_name = merge_table->file_name;
            merge_table->bloom_filter->Write();
            delete merge_table;

            string last_file_name = "";
            while (it != log_tables->end()) {
                cout<< "In merge loop" << endl;
                merge_table = new LogTable();
                LogTable log_table1(file_name);
                LogTable::Merge2(merge_table, log_table1, *(it));
                log_table1.Discard();
                it->Discard();
                file_name = merge_table->file_name;
                
                merge_table->bloom_filter->Write();
                delete merge_table;
                it++;
            }

        }
        
        static LogTable* Merge2(LogTable* result, LogTable &log_table1, LogTable &log_table2) { 
            
            LogTable *merged_table = result;
            LogTable::Iterator *it1 = new Iterator(&log_table1);
            LogTable::Iterator *it2 = new Iterator(&log_table2);

            Record *record1, *record2;
            
            record1 = it1->Move();
            record2 = it2->Move();
            
           while (record1 != 0 && record2 != 0) {    
                if (record1->get_key()
                            .compare(record2->get_key()) < 0) {
                    merged_table->Write(*record1);
                    record1 = it1->Move();
                } else if (record1->get_key()
                            .compare(record2->get_key()) > 0) {
                    merged_table->Write(*record2);
                    record2 = it2->Move();
                } else {
                    if (record1->get_value().get_time() > record2->get_value().get_time()) {
                        merged_table->Write(*record1);
                    } else {
                        merged_table->Write(*record2);
                    }
                    record1 = it1->Move();
                    record2 = it2->Move();
                }
            } 

        

            while (record1 != 0) {
                merged_table->Write(*record1);
                record1 = it1->Move();
            }

            while (record2 != 0) {
                merged_table->Write(*record2);
                record2 = it2->Move();
            }

            return merged_table;

        }

};