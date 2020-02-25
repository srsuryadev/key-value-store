#include <string>
#include <iostream>
#include <sys/time.h>
#include "kvstore.h"
#include <signal.h>
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

struct timeval stop, start;
static volatile int A = 0, B = 0, C = 0;

class KVStoreImpl final : public KVStore::Service {
    private:
        KeyValueStore db;

    public:

        Status set(ServerContext *context, const KeyValueRequest *key_val_req, SetResponse *set_res) override {
            A++;
            string key = key_val_req->key();
            string value = key_val_req->value();

            bool result = db.set(key, value);
            set_res->set_issuccessful(result);

            return Status::OK;
        }
Status getPrefix(ServerContext *context, const KeyRequest *key_req, ServerWriter<ValueResponse> *res_writer) override {
            C++;
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
    gettimeofday(&stop, NULL);
    cout<< (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec<<endl;
    cout<<"Server up and running on port: "<<address<<endl;
    server->Wait();
}

void handler(int dummy) {
    cout<<"\nTotal SET: "<<A<<endl;
    cout<<"Total GET: "<<B<<endl;
    cout<<"Total GET_PREFIX: "<<C<<endl;
    exit(0);
}

int main(int argc, char** argv) {
    gettimeofday(&start, NULL);
    signal(SIGINT, handler);
    runServer();

    return 0;
}
