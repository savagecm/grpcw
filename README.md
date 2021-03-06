# grpc cpp wrapper

# build status    
## master branch [![Build Status](https://travis-ci.org/savagecm/grpcw.svg?branch=master)](https://travis-ci.org/savagecm/grpcw)
      
## dev branch [![Build Status](https://travis-ci.org/savagecm/grpcw.svg?branch=dev)](https://travis-ci.org/savagecm/grpcw)

## badges    
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=savagecm_grpcw&metric=ncloc)](https://sonarcloud.io/dashboard?id=savagecm_grpcw)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=savagecm_grpcw&metric=alert_status)](https://sonarcloud.io/dashboard?id=savagecm_grpcw)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=savagecm_grpcw&metric=code_smells)](https://sonarcloud.io/dashboard?id=savagecm_grpcw)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=savagecm_grpcw&metric=coverage)](https://sonarcloud.io/dashboard?id=savagecm_grpcw)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=savagecm_grpcw&metric=duplicated_lines_density)](https://sonarcloud.io/dashboard?id=savagecm_grpcw)




## this project provide a wrapper of grpc for both c++98 and c++11 user    
the goal is to forget the grpc/protobuf detail infomation.
just need to provide the IDL file and then register service, do not need to care about the grpc.

## dependancy    
1. protobuf
2. grpc
3. C++11(optional)

## How to use    

1. just need to put your proto file under protos/ 
2. ./autogen.sh
3. mkdir build&&cd build&&cmake ..&&make

### how to use server    
just need three steps:
1. create a server with your service name +Server. For example:
```
GreeterServer server;
```
2. register RPC call function. For example:
```
server.register_rpc_SayHello(your function);
```
function arg is your request type and return your return type   


3. call Run. For example
```
server.Run("127.0.0.1", 2222);
```
### how to use client
just need three steps:
1. create a client with your service name + Client. For example:
```
GreeterClient greeter(grpc::CreateChannel("127.0.0.1:2222", grpc::InsecureChannelCredentials()));
```
2. call init(). For example:
```
greeter.init();
```
3. make rpc call. For example:
```
greeter.SayHello(request, your callback function);
```
the callback function definition: arg: your return type. return: void


## none C++11 
see example: wrapper_example.cpp
## support
now only support async unary call. Will add more if anyone need it. 
Email me : savagecm@qq.com