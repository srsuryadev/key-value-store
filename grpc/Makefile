LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl, -lgrpc++_reflection -Wl,\
           -ldl

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: kvclient kvserver

kvclient: kvstore.pb.o kvstore.grpc.pb.o kvclient.o
	$(CXX) $^ $(LDFLAGS) -o $@

kvserver: kvstore.pb.o kvstore.grpc.pb.o kvserver.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h kvclient kvserver
