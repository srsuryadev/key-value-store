#include "record.h"
  
void Record::set_key(string key) {
    this->key = key;
}

void Record::set_value(Value value) {
    this->value = value;
}

string Record::get_key() {
    return this->key;
}

Value Record::get_value() {
    return this->value;
}

void Record::write(ofstream *out) {
    unsigned int checksum = CalculateChecksum(*this);
    (*out) << checksum;
    (*out) << key.size();
    out->write(key.data(), key.size());
    string val = value.get_value();
    (*out) << val.size();
    out->write(val.data(), val.size());
    (*out) << value.get_time();
    (*out) << value.is_deleted();
}

void Record::write(FILE* &out) {
    unsigned int checksum = CalculateChecksum(*this);
    fwrite(&checksum, sizeof(checksum), 1, out);
    int key_size = key.size();
    fwrite(&key_size, sizeof(key_size), 1, out);
    fwrite(key.data(), key_size, 1, out);
    int val_size = value.get_value().size();
    fwrite(&val_size, sizeof(val_size), 1, out);
    fwrite(value.get_value().data(), val_size, 1, out);
    long ts = value.get_time();
    fwrite(&ts, sizeof(ts), 1, out);
    bool deleted_flag = value.is_deleted();
    fwrite(&deleted_flag, sizeof(deleted_flag), 1, out);
cout<<"File write complete\n";
}

bool Record::read(ifstream *in) {
cout<<"Reading REcord\n";
    //if(in->eof())
    //    return false;

    unsigned int file_checksum;
    (*in) >> file_checksum;
    if(in->fail())
        return false;
cout<<"Checksum read\n";    
    int str_size;
    (*in) >> str_size;
    if(in->fail())
        return false;
    string key;
    key.resize(str_size);
    in->read(&key[0], str_size);
    if(in->fail())
        return false;
cout<<"Key read\n";

    (*in) >> str_size;
    if(in->fail())
        return false;
    string val;
    val.resize(str_size);
    in->read(&val[0], str_size);
    if(in->fail())
        return false;
cout<<"Val read\n";

    long ts;
    (*in) >> ts;
    if(in->fail())
        return false;
cout<<"Ts read\n";

    bool delete_flag;
    (*in) >> delete_flag;
    if(in->fail())
        return false;
cout<<"Flag read\n";

    Value value = Value(val, ts, delete_flag);
    this->key = key;
    this->value = value;
    
    if(file_checksum != CalculateChecksum(*this)) {
        cout<<"CHECKSUM MISMATCH\n";
        return false;
    } else {
        cout<<"CHECKSUM MATCH\n";
        return true;
    }
}

unsigned int Record::CalculateChecksum(Record record) {
    boost::crc_32_type crc32;
    string key = record.get_key();
    crc32.process_bytes(key.data(), key.length());
    crc32.process_bytes(record.get_value().get_value().data(), record.get_value().get_value().length());
    int timestamp = record.get_value().get_time();
    crc32.process_bytes(&timestamp, sizeof(timestamp));
    bool delete_flag = record.get_value().is_deleted();
    crc32.process_bytes(&delete_flag, sizeof(delete_flag));
    return crc32.checksum();
}

