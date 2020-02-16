#include "value.h"

using namespace std;


string Value::get_value() {
    return this->value;
}

long Value::get_time() {
    return this->timestamp;
}

bool Value::is_deleted() {
    return this->deleted;      
}

void Value::set_value(string value) {
    this->value = value;
    this->timestamp = time(0);
    this->deleted = false;
}

void Value::mark_deleted() {
    this->deleted = true;
}

//Not Required. Can be removed
void Value::write(ofstream out) {

    size_t len = value.size();
    out.write((char*)&len, sizeof(size_t));
    out.write(value.c_str(), len);
    out.write((char*)&timestamp, sizeof(long));
    out.write((char*)&deleted, sizeof(bool));
}

//Not Required. Can be removed
void Value::read(ifstream in) {
    size_t len;
    in.read((char*)&len, sizeof(size_t));
    char* value_arr = new char[len + 1];
    in.read(value_arr, len);
    value_arr[len] = '\0';
    this->value = value_arr;
    delete [] value_arr;

    in.read((char*)&timestamp, sizeof(long));
    in.read((char*)&deleted, sizeof(bool));
}