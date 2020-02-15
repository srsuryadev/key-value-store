#include <string>
#include <iostream>
using namespace std;

#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using kvstore::KVStore;
using kvstore::KeyRequest;
using kvstore::KeyValueRequest;
using kvstore::SetResponse;
using kvstore::ValueResponse;

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

void runClient() {
    const string address("0.0.0.0:5000");
    KVStoreClient KVStoreClient(grpc::CreateChannel(
        address, 
        grpc::InsecureChannelCredentials()
    ));
    string key = "ABCD";
    string value = "1234";
    cout<<"Setting key: "<<key<<" Value: "<<value<<endl;
    cout<<KVStoreClient.set(key, value)<<endl;

    cout<<"Getting key: "<<key<<endl;
    cout<<KVStoreClient.get(key)<<endl;

    cout<<"Getting Prefixes for key: "<<key<<endl;
    vector<string> result;
    if(KVStoreClient.getPrefix(key, result)) {
        printStringVector(result);
    }
}

int main(int argc, char *argv[]) {
    runClient();
    
    return 0;
}
