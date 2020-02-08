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

void Record::write(ofstream out) {
    size_t len = value.size();
    out.write(&len, sizeof(size_t));
    out.write(value.c_str, len);
    value.write(out);
}

void Record::read(ifstream in) {
    size_t len;
    in.read(&len, sizeof(size_t));
    char* value_arr = new char[len + 1];
    in.read(value_arr, len);
    value_arr[len] = '\0';
    this.value = value_arr;
    delete [] value_arr;
    value.read(in);
}