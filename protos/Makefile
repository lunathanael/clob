PROTO_FILES = $(wildcard *.proto)

PB_FILES = $(PROTO_FILES:.proto=.pb.cpp) $(PROTO_FILES:.proto=.pb.h)
GRPC_FILES = $(PROTO_FILES:.proto=.grpc.pb.cpp) $(PROTO_FILES:.proto=.grpc.pb.h)

.PHONY: all clean

all: $(PB_FILES) $(GRPC_FILES)

%.pb.cpp %.pb.h: %.proto
	protoc -I . --cpp_out=. $<
	mv $*.pb.cc $*.pb.cpp

%.grpc.pb.cpp %.grpc.pb.h: %.proto
	protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<
	mv $*.grpc.pb.cc $*.grpc.pb.cpp

clean:
	rm -f *.pb.cpp *.pb.h *.grpc.pb.cpp *.grpc.pb.h *.pb.cc *.grpc.pb.cc