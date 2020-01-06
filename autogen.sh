#!/bin/bash
PROJECT_SOURCE_DIR="$PWD"
PROTOS_PATH="${PROJECT_SOURCE_DIR}/protos"

# gen proto file
gen_path="${PROJECT_SOURCE_DIR}"


mkdir -p ${PROTOS_PATH}/gen
mkdir -p ${PROJECT_SOURCE_DIR}/src/gen_target
mkdir -p ${PROJECT_SOURCE_DIR}/include/gen_target
mkdir -p ${PROJECT_SOURCE_DIR}/include/grpc
mkdir -p ${PROJECT_SOURCE_DIR}/src/grpc
for PROTOS in `find ${PROTOS_PATH} -name *.proto`
do
    echo "process file : ${PROTOS}"
    protoc  --cpp_out "${PROTOS_PATH}/gen" --proto_path "${PROTOS_PATH}" ${PROTOS}
    protoc  --grpc_out "${PROTOS_PATH}/gen" --proto_path "${PROTOS_PATH}" --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ${PROTOS}

   # cp ${PROTOS_PATH}/*.cc ${PROJECT_SOURCE_DIR}/src/gen_target
   # cp ${PROTOS_PATH}/*.h ${PROJECT_SOURCE_DIR}/include/gen_target
   # rm ${PROTOS_PATH}/*.cc
   # rm ${PROTOS_PATH}/*.h
done

for genfile in `find ${PROTOS_PATH}/gen -name *.cc`
do 
    
    cp -r ${genfile} ${PROJECT_SOURCE_DIR}/src/gen_target/
done

for genfile in `find ${PROTOS_PATH}/gen -name *.h`
do 
    
    cp -r ${genfile} ${PROJECT_SOURCE_DIR}/include/gen_target/
done
# gen code 


generate_proto_path="gen_target"
out_header_path="${PROJECT_SOURCE_DIR}/include/grpc"
out_src_path="${PROJECT_SOURCE_DIR}/src/grpc"

for PROTOS in `find ${PROTOS_PATH} -name *.proto`
do
    exec_cmd="protos/gen-all-grpc-async-code.py -p ${PROTOS} -gp ${generate_proto_path} -ohp ${out_header_path} -osp ${out_src_path} -tp ${PROTOS_PATH}"

    echo "exe cmd : ${exec_cmd}"
    ${exec_cmd}
done
