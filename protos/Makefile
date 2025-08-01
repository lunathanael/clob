PROTO_FILES = $(wildcard *.proto)

PB_FILES = $(PROTO_FILES:.proto=.pb.cc) $(PROTO_FILES:.proto=.pb.h)
GRPC_FILES = $(PROTO_FILES:.proto=.grpc.pb.cc) $(PROTO_FILES:.proto=.grpc.pb.h)

.PHONY: all clean

all: $(PB_FILES) $(GRPC_FILES)

%.pb.cc %.pb.h: %.proto
	protoc -I . --cpp_out=. $<

%.grpc.pb.cc %.grpc.pb.h: %.proto
	protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

clean:
	rm -f *.pb.cc *.pb.h *.grpc.pb.cc *.grpc.pb.h