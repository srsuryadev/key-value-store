#include <string>
#include <iostream>
using namespace std;

#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using kvstore::KVStore;
using kvstore::KeyRequest;
using kvstore::KeyValueRequest;
using kvstore::SetResponse;
using kvstore::ValueResponse;

class KVStoreImpl final : public KVStore::Service {
    public:
        Status set(ServerContext *context, const KeyValueRequest *key_val_req, SetResponse *set_res) override {
            string key = key_val_req->key();
            string value = key_val_req->value();
            cout<<"Received SET request --- Key: "<<key<<" Value: "<<value<<endl;
            set_res->set_issuccessful(true);
            return Status::OK;
        }

        Status get(ServerContext *context, const KeyRequest *key_req, ValueResponse *val_res) override {
            string key = key_req->key();
            cout<<"Received GET request --- Key: "<<key<<endl;
            val_res->set_value("Sample Value");
            return Status::OK;
        }

        Status getPrefix(ServerContext *context, const KeyRequest *key_req, ServerWriter<ValueResponse> *res_writer) override {
            string key = key_req->key();
            cout<<"Received GET_PREFIX request --- Key: "<<key<<endl;
            int i;
            for(i=0; i<10; i++) {
                string val = "Value" + to_string(i);
                cout<<"Writing "<<val<<" to stream\n";
                ValueResponse val_res;
                val_res.set_value(val);
                res_writer->Write(val_res);
            }
            return Status::OK;
        }
};

void runServer() {
    ServerBuilder server_builder;
    const string address("0.0.0.0:5000");
    server_builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    KVStoreImpl KVStoreService;
    server_builder.RegisterService(&KVStoreService);
    unique_ptr<Server> server(server_builder.BuildAndStart());
    cout<<"Server up and running on port: "<<address<<endl;
    server->Wait();
}

int main(int argc, char** argv) {
    runServer();

    return 0;
}
