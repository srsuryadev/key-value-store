//#include "wal.h"
//#include "record.h"
#include "log_table.h"
#include <iostream>
using namespace std;

int main() {

    LogTable *log_table = new LogTable();
    cout << "good: " << log_table->file_name << endl;
    return 0;

    /*
    WAL *wal = WAL::GetInstance();
    wal->CreateWriteStream();
    
    Record record;
    Value value;
    value.set_value("Val1");
    record.set_key("Key1");
    record.set_value(value);
    wal->Append(record);
    cout<<"WROTE RECORD\n";

    Record record2;
    Value value2;
    value2.set_value("Val2");
    record2.set_key("Key2");
    record2.set_value(value2);
    wal->Append(record2);
    cout<<"WROTE RECORD\n";

    Record record3;
    Value value3;
    value3.set_value("Val3");
    record3.set_key("Key3");
    record3.set_value(value3);
    wal->Append(record3);
    cout<<"WROTE RECORD\n";
    wal->CloseWriteStream();

    WAL::Iterator wal_iter(wal);
    wal->OpenReadStream();
    while(wal_iter.HasNext()) {
        Record rec = wal_iter.Next();
        cout<<"READ KEY: "<<rec.get_key()<<endl;
    }

    wal->CloseReadStream();
    wal->Discard();
    return 0;*/
}