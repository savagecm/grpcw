#!/usr/bin/python
import sys
import re
import os
import pprint

input_file = ""
output_header = ""
output_source = ""
header_template = ""
source_template = ""
generated_protobuf_header = ""
generated_grpc_header = ""
service = ""
rpcs = []
single_replace = {}

def replace(lines, replace_args):
    new_lines = lines
    for key, value in replace_args.iteritems():
        if re.search("^%.+%$", key):
            new_lines = re.sub(key, value, new_lines)
    return new_lines

def generate_file(file_handler, template):
    line_buffer = ""
    repeat = False
    stream_req_repeat = False
    stream_resp_repeat = False
    stream_req_resp_repeat = False

    for line in template:
        if re.search("%repeat_start%", line):
            repeat = True
            continue

        if re.search("%stream_req_repeat_start%", line):
            stream_req_repeat = True
            continue
        if re.search("%stream_resp_repeat_start%", line):
            stream_resp_repeat = True
            continue
        if re.search("%stream_req_resp_repeat_start%", line):
            stream_req_resp_repeat = True
            continue

        if re.search("%repeat_end%", line):
            lines = ""
            for rpc in rpcs:
                #            rpc = {"%function_name%": "", "%function_argument_type%": "", "streamed_function_argument_type": False, "%return_type%": "", "streamed_return_type": False}
                if repeat and not rpc["streamed_function_argument_type"] and not rpc["streamed_return_type"]:
                    replace_args = rpc
                elif stream_req_repeat and rpc["streamed_function_argument_type"] and not rpc["streamed_return_type"]:
                    replace_args = rpc
                elif stream_resp_repeat and not rpc["streamed_function_argument_type"] and rpc["streamed_return_type"]:
                    replace_args = rpc
                elif stream_req_resp_repeat and rpc["streamed_function_argument_type"] and rpc["streamed_return_type"]:
                    replace_args = rpc
                else:
                    continue
                replace_args.update(single_replace)
                lines += replace(line_buffer, replace_args)
            file_handler.write(lines)
            line_buffer = ""
            repeat = False
            stream_req_repeat = False
            stream_resp_repeat = False
            stream_req_resp_repeat = False
            continue

        if repeat or stream_req_repeat or stream_resp_repeat or stream_req_resp_repeat:
            line_buffer += line
        else:
            lines = replace(line, single_replace)
            file_handler.write(lines)

if __name__ == '__main__':
    for i in range(1, len(sys.argv)):
        arg = sys.argv[i]
        if arg == "-i":
            input_file = sys.argv[i+1]
        if arg == "-ph":
            generated_protobuf_header = sys.argv[i+1]
        if arg == "-gh":
            generated_grpc_header = sys.argv[i+1]
        if arg == "-oh":
            output_header = sys.argv[i+1]
        if arg == "-os":
            output_source = sys.argv[i+1]
        if arg == "-ht":
            header_template = sys.argv[i+1]
        if arg == "-st":
            source_template = sys.argv[i+1]

    if len(input_file) == 0 or len(output_header) == 0 or len(output_source) == 0 or len(header_template) == 0 or len(source_template) == 0 or len(generated_protobuf_header) == 0:
        print("Usage: generate-grpc-async-client.py -i <.proto file> -ph <generated protobufheader file> -oh <output cpp header file> -os <output cpp source file> -ht <cpp header template> -st <cpp source template>")
        print("input_file is "+input_file+", output_header is "+output_header+", output_source is "+output_source+", header_template is "+header_template+", source_template is "+source_template+", generated_protobuf_header"+ generated_protobuf_header)
        sys.exit(1)
    single_replace["%namespaces%"] = ""
    single_replace["%namespaces_end%"] = ""

    proto_file = open(input_file, 'r')
    for line in proto_file:
        line = line.strip()
        if (line[:8] == "service "):
            service = line.split()[1]
            single_replace["%service%"] = service
        if (line[:8] == "package "):
            m = re.search("package +(.+) *;", line)
            package = m.group(1)
            namespaces = package.split(".")
            for namespace in namespaces:
                single_replace["%namespaces%"] += "namespace " + \
                    namespace + " { "
                single_replace["%namespaces_end%"] += "} "
        if (line[:4] == "rpc "):
            rpc = {"%function_name%": "", "%function_argument_type%": "",
                   "streamed_function_argument_type": False, "%return_type%": "", "streamed_return_type": False}
            m = re.search("rpc +(.+)\((.+)\) +returns +\((.+)\)", line)
            function_name = m.group(1).strip()
            function_argument_type = m.group(2).strip()
            return_type = m.group(3).strip()

            m = re.search("(.+) +(.+)", function_argument_type)
            if m:
                if m.group(1).strip() != "stream":
                    sys.exit(1)
                function_argument_type = m.group(2).strip()
                rpc["streamed_function_argument_type"] = True

            m = re.search("(.+) +(.+)", return_type)
            if m:
                if m.group(1).strip() != "stream":
                    print "Something went wrong: " + line
                    sys.exit(1)
                return_type = m.group(2).strip()
                rpc["streamed_return_type"] = True
            rpc["%function_name%"] = function_name
            rpc["%function_argument_type%"] = function_argument_type
            rpc["%return_type%"] = return_type
            rpcs.append(rpc)
    proto_file.close()

    if not single_replace.has_key("%service%"):
        print(" file " + input_file + " do not have service, return")
        sys.exit()

    hpp_template_file = open(header_template, "r")
    hpp_template = []
    for line in hpp_template_file:
        hpp_template.append(line)
    hpp_template_file.close()

    src_template_file = open(source_template, "r")
    src_template = []
    for line in src_template_file:
        src_template.append(line)
    src_template_file.close()





    single_replace["%generated_grpc_header%"] = generated_grpc_header
    single_replace["%generated_protobuf_header%"] = generated_protobuf_header
    single_replace["%grpc_async_header%"] = os.path.basename(output_header)

    header_file = open(output_header, "w+")
    generate_file(header_file, hpp_template)
    header_file.close()

    source_file = open(output_source, "w+")
    generate_file(source_file, src_template)
    source_file.close()
