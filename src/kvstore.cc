#include "kvstore.h"


mutex KeyValueStore::_mutex;

KeyValueStore::KeyValueStore() {
    first_skip_list = new SkipList();
    second_skip_list = new SkipList();
    current_skip_list = first_skip_list;

    //While recover merge the log tables;

     _mutex.lock();

    // collate the existing logtables on recover
    LogTable::Merge();

    // If the WAL has records use it to recover
    WAL *wal = WAL::GetInstance();
    WAL::Iterator *it = new WAL::Iterator(wal);
    while (it->HasNext()) {
        Record record = it->Next();
        string key = record.get_key();
        string value = record.get_value().get_value();

        current_skip_list->put(key, value);
    }

    _mutex.unlock();
}

bool KeyValueStore::set(string key, string value) {
    cout<<"Received SET request --- Key: "<<key<<" Value: "<<value<<endl;
    WAL* wal = WAL::GetInstance();

    Record record;
    record.set_key(key);
    Value _val;
    _val.set_value(value);
    record.set_value(_val);
    wal->Append(record);

    current_skip_list->put(key, value);
    if (++current_skip_list->count == CUT_OFF_COUNT) {
        // TODO: force first list to disk
        // use the other list
        SkipList* prev_skip_list;
        if ((uintptr_t)current_skip_list == (uintptr_t)first_skip_list) {
            current_skip_list = second_skip_list;
            prev_skip_list = second_skip_list;
        } else {
            current_skip_list = first_skip_list;
            prev_skip_list = first_skip_list;
        }

        
        //TODO: In future, we can use another thread to put
        // the skiplist contents to the logtable;
        LogTable logtable;

        list<Record> records = prev_skip_list->get_all_data();
        logtable.Write(records);
        //Truncate the existing WAL and create a fresh
        wal->OpenReadStream();
        
    }
    return true;
}

string KeyValueStore::get(string key) {
    cout<<"Received GET request --- Key: "<<key<<endl;
    string value = current_skip_list->get(key);
    if (value.size() == 0) {
        Record* record = LogTable::Get(key);
        return record->get_value().get_value();
    }
    return current_skip_list->get(key);
}

vector<string> KeyValueStore::get_prefix(string prefix_key) {
    vector<string> ret_val;    
    cout<<"Received GET request for prefix --- Key: "<< prefix_key <<endl;
    return current_skip_list->get_keys_for_prefix(prefix_key);    
}