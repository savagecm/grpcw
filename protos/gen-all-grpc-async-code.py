#!/usr/bin/python
import os
import sys
proto_path = ""
generated_proto_path = ""
output_header_path = ""
output_source_path = ""
header_template = ""
source_template = ""

if __name__ == '__main__':
    for i in range(1, len(sys.argv)):
        arg = sys.argv[i]
        if arg == "-pp":
            # proto file path
            proto_path = sys.argv[i + 1]
        if arg == "-gp":
            generated_proto_path = sys.argv[i + 1]
        if arg == "-ohp":
            output_header_path = sys.argv[i + 1]
        if arg == "-osp":
            output_source_path = sys.argv[i + 1]

    if len(proto_path) == 0 or len(generated_proto_path) == 0 or len(
            output_header_path) == 0 or len(output_source_path) == 0:
        print("Usage: , going to exit"+'\n'+proto_path+'\n'+generated_proto_path+'\n'+output_header_path+'\n'+output_source_path)
        sys.exit(1)
    protos = map(
        lambda protoName: protoName.split('.')[0],
        filter(lambda fname: "proto" in fname and "Types" not in fname, os.listdir(proto_path)))
    print('now start to process')
    for file in protos:
        print('process service : ' + file)
        clientCmdStr = 'protos/generate-grpc-async-code.py ' + '-i ' + proto_path + '/' + file + '.proto ' + '-ph ' + generated_proto_path + '/' + file + '.pb.h ' + '-gh ' + generated_proto_path + '/' + file + '.grpc.pb.h ' + '-oh ' + output_header_path + '/' + file + 'Client' + '.hpp ' + '-os ' + output_source_path + '/' + file + 'Client' + '.cpp ' + '-ht ' + proto_path + '/' + 'unary_client_tmp.hpp ' + '-st ' + proto_path + '/' + 'unary_client_tmp.cpp '
        print('exec : ' + clientCmdStr)
        os.system(clientCmdStr)
        os.system('protos/generate-grpc-async-code.py ' + '-i ' + proto_path + '/' +
                  file + '.proto ' + '-ph ' + generated_proto_path + '/' +
                  file + '.pb.h ' + '-gh ' + generated_proto_path + '/' +
                  file + '.grpc.pb.h ' + '-oh ' + output_header_path + '/' +
                  file + 'Server' + '.hpp ' + '-os ' + output_source_path +
                  '/' + file + 'Server' + '.cpp ' + '-ht ' + proto_path + '/' +
                  'unary_server_tmp.hpp ' + '-st ' + proto_path + '/' +
                  'unary_server_tmp.cpp ')
