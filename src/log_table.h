#include<stdint.h>
#include <map>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <list>
#include <ctime>
#include "record.h"

using namespace std;

class LogTable {

    private:

        
        static const  string file_name_;
        static const  boost::filesystem::path data_path_;


        std::ofstream out_logtable_;
        std::ifstream in_logtable_;

        string NextFileName() {
            string new_file = file_name_ + std::to_string(time(0) * 1000);
            return new_file;
        }

        void Create() {
            this->file_name = this->NextFileName();
            out_logtable_.open(this->file_name, std::ofstream::out);
        }

        void Open(string file_name) {
            this->file_name = file_name;
            in_logtable_.open(file_name, std::ifstream::in);
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

       
        string file_name;

        LogTable();

        ~LogTable();

        LogTable(string file_name);

        void Write(list<Record> &records);
        void Write(Record &record);

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
                    if (!logtable->in_logtable_.is_open())
                        return false;

                    next_record = new Record;
                    if (!next_record->read(&logtable->in_logtable_))
                        return false;
                    //TODO do check for end of file or checksum
                    return true;
                }

                Record* Next(){
                    return next_record;
                }
        };



        static list<Record>* Get(string key) {
            list<Record>* records = new list<Record>();
            list<LogTable>* log_tables = LogTable::GetLogTables();

            std::list<LogTable>::iterator it;
            for (it = log_tables->begin(); it != log_tables->end(); it++) {
                // TODO: Use bloom filter to skip the log tables

                // TODO: if present, seek to the nearest offset using the sparse index
                Record* record = it->Search(key);
                if (record) {
                    records->push_back(*record);
                }
            }

            return records;
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
            /*boost::filesystem::path p = current_path();
             boost::filesystem::directory_iterator it{p};
            while (it !=  boost::filesystem::directory_iterator{}) {
                LogTable* log_table = new LogTable(it->path().filename().string());
                log_tables->push_back(*log_table);
                std::cout << *it++ << '\n';
            }*/
            return log_tables;
        }
        
        static LogTable* Merge(LogTable &log_table1, LogTable &log_table2) { 
            
            LogTable *merged_table = new LogTable;
            
            LogTable::Iterator *it1 = new Iterator(&log_table1);
            LogTable::Iterator *it2 = new Iterator(&log_table2);

            Record record1, record2;
            if (it1->HasNext())
                record1 = *it1->Next();
            if (it2->HasNext())
                record2 = *it2->Next();

            do {

                while (record1.get_value().is_deleted() && it1->HasNext())
                    record1 = *it1->Next();

                while (record2.get_value().is_deleted() && it2->HasNext())
                    record2 = *it2->Next();
                
                if (record1.get_key()
                            .compare(record2.get_key()) < 0) {
                    merged_table->Write(record1);
                    record1 = *it1->Next();
                } else if (record1.get_key()
                            .compare(record2.get_key()) > 0) {
                    merged_table->Write(record2);
                    record2 = *it2->Next();
                } else {
                    if (record1.get_value().get_time() > record2.get_value().get_time()) {
                        merged_table->Write(record1);
                    } else {
                        merged_table->Write(record2);
                    }
                    record1 = *it1->Next();
                    record2 = *it2->Next();
                }
            }  while (it1->HasNext() && it2->HasNext());

            do {
                record1 = *it1->Next();
                merged_table->Write(record1);
            } while (it1->HasNext());

            do {
                record2 = *it2->Next();
                merged_table->Write(record2);
            } while (it1->HasNext());

            return merged_table;

        }

};