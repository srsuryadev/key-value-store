#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"
using namespace std;

#define WRITE_OP 0
#define GET_OP 1
#define GET_PREFIX 2

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using kvstore::KVStore;
using kvstore::KeyRequest;
using kvstore::KeyValueRequest;
using kvstore::SetResponse;
using kvstore::ValueResponse;

int NUM_THREADS;
int NUM_OP_PER_THREAD;
int MAX_LEN_STRING;
int MODE;

void generate_random_string(char *s, int len) {
    static const char valid_chars[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "012345678_$-@#:;,.";
    int i;
    for (i = 0; i < len; i++) {
        s[i] = valid_chars[rand() % 61];
    }
    s[i] = '\0';
}

string generate_random_string() {
    int len = 0;
    
    while (len == 0)
        len = rand() % MAX_LEN_STRING;
    char* s = (char*) malloc(len * sizeof(char) + 1);
    generate_random_string(s, len);
    string result = s;
    return result;
}

class KVStoreClient {
    private:
        unique_ptr<KVStore::Stub> stub_;

    public:
        KVStoreClient(shared_ptr<Channel> channel) : stub_(KVStore::NewStub(channel)) {}

        bool set(const string key, const string value) {
            ClientContext context;
            KeyValueRequest req;
            req.set_key(key);
            req.set_value(value);
            SetResponse res;
            Status status = stub_->set(&context, req, &res);
            if(status.ok()){
                return res.issuccessful();
            } else {
                cout<<status.error_code()<<": "<<status.error_message()<<endl;
                return false;
            }
        }

        string get(const string key) {
            ClientContext context;
            KeyRequest req;
            req.set_key(key);
            ValueResponse res;
            Status status = stub_->get(&context, req, &res);
            if(status.ok()){
                return res.value();
            } else {
                cout<<status.error_code()<<": "<<status.error_message()<<endl;
                return "";
            }
        }

        bool getPrefix(const string key, vector<string> &result) {
            ClientContext context;
            KeyRequest req;
            req.set_key(key);
            unique_ptr<ClientReader<ValueResponse>> res_reader(stub_->getPrefix(&context, req));
            ValueResponse val_res;
            while(res_reader->Read(&val_res)) {
                //cout<<"Received value in stream: "<<val_res.value()<<endl;
                result.push_back(val_res.value());
            }
            Status status = res_reader->Finish();
            if (status.ok()) {
                return true;
            } else {
                cout<<"getPrefix RPC Failed\n";
                return false;
            }
        }
};

void printStringVector(vector<string> &V) {
    int i;
    for(i=0; i<V.size(); i++) {
        cout<<V[i]<<" ";
    }
    cout<<endl;
}

void* runClient(void *args) {
    const string address("0.0.0.0:5000");
    KVStoreClient KVStoreClient(grpc::CreateChannel(
        address, 
        grpc::InsecureChannelCredentials()
    ));
    int i;
    vector<string> keys;
    for(i=0; i<NUM_OP_PER_THREAD; i++) {
        int op = WRITE_OP;
        if(MODE == 2) {
            op = rand() % 2;
        } else if (MODE == 1) {
            op = GET_OP;
        }

        if (op == WRITE_OP) {
            string key = generate_random_string();
            string value = generate_random_string();          
            keys.push_back(key);
            cout<< "Setting --- key: " << key << " Value: " << value << endl;
            cout<<"Success: "<<KVStoreClient.set(key, value)<<endl;
        } else if (op == GET_OP && keys.size() > 0) {
            int index = rand() % keys.size();
            string key = keys[index];
            cout << "Getting --- value: " << KVStoreClient.get(key) << " for key " << key << endl;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        cout<<"Insufficient number of arguments\n";
        return 0;
    }
    NUM_THREADS = stoi(argv[1]);
    NUM_OP_PER_THREAD = stoi(argv[2]);
    MAX_LEN_STRING = stoi(argv[3]);
    MODE = stoi(argv[4]);

    pthread_t threads[NUM_THREADS];
    int i;
    for(i=0; i<NUM_THREADS; i++) {
        int rc = pthread_create(&threads[i], NULL, runClient, NULL);
        if (rc) {
            cout << "Not able to create thread" << endl;
            exit(-1);
        }
    }
    for(i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
