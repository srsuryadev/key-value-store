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

        const string file_name_;
        const boost::filesystem::path data_path_;

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
            record.read(this->in_logtable_);
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

        void Write(list<Record> records);
        void Write(Record record);
        
        LogTable Merge(LogTable* logtable);

        Record Search(string key);
        list<LogTable>& SearchPrefix(string key);


        static list<LogTable>& GetLogTables();

        static list<Record>& Get(string key);
        static list<Record>& GetPrefix(string key);
        

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
                    if (!next_record->read(logtable->in_logtable_))
                        return false;
                    //TODO do check for end of file or checksum
                    return true;
                }

                Record Next(){
                    Record record = *next_record;
                    delete next_record;
                    return record;
                }
        };
};