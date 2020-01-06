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
        if arg == "-p":
            # proto file
            proto_file = sys.argv[i + 1]
        if arg == "-gp":
            generated_proto_path = sys.argv[i + 1]
        if arg == "-ohp":
            output_header_path = sys.argv[i + 1]
        if arg == "-osp":
            output_source_path = sys.argv[i + 1]
        if arg == "-tp":
            output_template_path = sys.argv[i + 1]

    if len(proto_file) == 0 or len(generated_proto_path) == 0 or len(
            output_header_path) == 0 or len(output_source_path) == 0:
        print("Usage: , going to exit" + '\n' + proto_file + '\n' +
              generated_proto_path + '\n' + output_header_path + '\n' +
              output_source_path)
        sys.exit(1)

    #   lambda protoName: protoName.split('.')[0],

    file = os.path.basename(proto_file).split('.')[0]
    print('now start to process file ' + proto_file + ', file name is ' + file)

    clientCmdStr = 'protos/generate-grpc-async-code.py ' + '-i ' + proto_file + ' -ph ' + generated_proto_path + '/' + file + '.pb.h ' + '-gh ' + generated_proto_path + '/' + file + '.grpc.pb.h ' + '-oh ' + output_header_path + '/' + file + 'Client' + '.hpp ' + '-os ' + output_source_path + '/' + file + 'Client' + '.cpp ' + '-ht ' + output_template_path + '/' + 'unary_client_tmp.hpp ' + '-st ' + output_template_path + '/' + 'unary_client_tmp.cpp '
    print('exec : ' + clientCmdStr)
    os.system(clientCmdStr)
    os.system('protos/generate-grpc-async-code.py ' + '-i ' + proto_file +
              ' -ph ' + generated_proto_path + '/' + file + '.pb.h ' + '-gh ' +
              generated_proto_path + '/' + file + '.grpc.pb.h ' + '-oh ' +
              output_header_path + '/' + file + 'Server' + '.hpp ' + '-os ' +
              output_source_path + '/' + file + 'Server' + '.cpp ' + '-ht ' +
              output_template_path + '/' + 'unary_server_tmp.hpp ' + '-st ' +
              output_template_path + '/' + 'unary_server_tmp.cpp ')
