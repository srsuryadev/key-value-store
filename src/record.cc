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
    out->write((char *) &checksum, sizeof(checksum));
    size_t key_size = key.size();
    out->write((char *) &key_size, sizeof(key_size));
    out->write((char *) key.data(), key_size);
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
}

bool Record::read(ifstream *in) {
    cout<<in->eof()<<endl;
    if(in->eof())
        return false;

    unsigned int file_checksum;
    (*in) >> file_checksum;
    cout<<in->fail()<<endl;
    if(in->fail())
        return false;
    cout<<file_checksum<<endl;
    int str_size;
    (*in) >> str_size;
    if(in->fail())
        return false;
    string key;
    key.resize(str_size);
    in->read(&key[0], str_size);
    if(in->fail())
        return false;

    (*in) >> str_size;
    if(in->fail())
        return false;
    string val;
    val.resize(str_size);
    in->read(&val[0], str_size);
    if(in->fail())
        return false;

    long ts;
    (*in) >> ts;
    if(in->fail())
        return false;

    bool delete_flag;
    (*in) >> delete_flag;
    if(in->fail())
        return false;

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

bool Record::read(FILE* &in) {
    //cout<<in->eof()<<endl;
    if(feof(in))
        return false;

    unsigned int file_checksum;
    size_t x = fread(&file_checksum, sizeof(file_checksum), 1, in);
    //cout<<in->fail()<<endl;
    if(x == 0 && (ferror(in) || feof(in)))
        return false;
    //cout<<file_checksum<<endl;

    int str_size;
    x = fread(&str_size, sizeof(str_size), 1, in);
    if(x == 0 && (ferror(in) || feof(in)))
        return false;

    string key;
    key.resize(str_size);
    x = fread(&key[0], str_size, 1, in);
    if(x == 0 && (ferror(in) || feof(in)))
        return false;

    x = fread(&str_size, sizeof(str_size), 1, in);
    if(x == 0 && (ferror(in) || feof(in)))
        return false;

    string val;
    val.resize(str_size);
    x = fread(&val[0], str_size, 1, in);
    if(x == 0 && (ferror(in) || feof(in)))
        return false;

    long ts;
    x = fread(&ts, sizeof(ts), 1, in);
    if(x == 0 && (ferror(in) || feof(in)))
        return false;

    bool delete_flag;
    x = fread(&delete_flag, sizeof(delete_flag), 1, in);
    if(x == 0 && (ferror(in) || feof(in)))
        return false;

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

