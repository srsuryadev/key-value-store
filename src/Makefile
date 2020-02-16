CC = g++
CFLAGS = -g -Wall -Wextra
RM = rm -rf
LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -lgrpc++_reflection\
           -ldl	-lboost_system
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17

BOOSTFLAGS = -lboost_system
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
DB = kvstore.o value.o record.o wal.o log_table.o skiplist.o

default: kvclient kvserver

kvclient: kvstore.pb.o kvstore.grpc.pb.o kvclient.o
	$(CXX) $^ $(LDFLAGS) -o $@

kvserver: kvstore.pb.o kvstore.grpc.pb.o $(DB) kvserver.o
	$(CXX) $^ $(LDFLAGS) -o $@

kvstore: kvstore.o value.o record.o wal.o log_table.o #main.o
	$(CC) $(CFLAGS) $^ -o $@

#%.o: %.cc
#	$(CC) $(CFLAGS) -c $<

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h kvclient kvserver
