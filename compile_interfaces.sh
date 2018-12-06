#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

protoc --proto_path=$DIR/src/vtserver --cpp_out=$DIR/src/vtserver --grpc_out=$DIR/src/vtserver --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $DIR/src/vtserver/vtserver_interface.proto
python3 -m grpc_tools.protoc --proto_path=$DIR/src/vtserver --python_out=$DIR/pyclient/vtclient --grpc_python_out=$DIR/pyclient/vtclient $DIR/src/vtserver/vtserver_interface.proto


