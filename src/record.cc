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
    //TODO: Shouldn't you write key here instead?
    /*size_t len = value.size();
    out.write(&len, sizeof(size_t));
    out.write(value.c_str, len);
    value.write(out);*/
    unsigned int checksum = CalculateChecksum(*this);
    out->write((char *) &checksum, sizeof(checksum));
    out->write((char *) this, sizeof(*this));
}

void Record::write(FILE* &out) {
    unsigned int checksum = CalculateChecksum(*this);
    fwrite(&checksum, sizeof(checksum), 1, out);
    fwrite(this, sizeof(*this), 1, out);
}

bool Record::read(ifstream *in) {
    /*size_t len;
    in.read(&len, sizeof(size_t));
    char* value_arr = new char[len + 1];
    in.read(value_arr, len);
    value_arr[len] = '\0';
    this.value = value_arr;
    delete [] value_arr;
    value.read(in);*/
    if(in->eof())
        return false;

    unsigned int file_checksum;
    in->read((char *) &file_checksum, sizeof(file_checksum));
    if(in->fail())
        return false;

    in->read((char *) this, sizeof(*this));
    if(in->fail())
        return false;
    
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
