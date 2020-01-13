#include "%generated_protobuf_header%"
#include "%generated_grpc_header%" 
#include "grpc/%grpc_async_header%"
#include "grpc/%grpc_async_wrapper_header%"

#include <grpcpp/grpcpp.h>
#include <memory>

#include "logger/logger.hpp"
%namespaces%
std::unique_ptr<%service%Server> %service%ServerWrapper_uptr;


%service%ServerWrapper::~%service%ServerWrapper()
{
    if(%service%ServerWrapper_uptr)
    {
        %service%ServerWrapper_uptr.reset(nullptr);
    }
}

%service%ServerWrapper::%service%ServerWrapper()
{
    std::unique_ptr<%service%Server> tmpServerWrapper_uptr( new %service%Server());
    %service%ServerWrapper_uptr = std::move(tmpServerWrapper_uptr);
}

void %service%ServerWrapper::Run(char* host, uint16_t port)
{

    std::string hostString(host);
    if(%service%ServerWrapper_uptr)
    {
        %service%ServerWrapper_uptr->Run(hostString, port);
    }    
}

%repeat_start%
void %service%ServerWrapper::register_rpc_%function_name%(%function_name%_cb user_callback_fn)
{
    if(%service%ServerWrapper_uptr)
    {
        std::function<void (%function_argument_type% const &, int index)> %function_name%_cb_11 = user_callback_fn;
        %service%ServerWrapper_uptr->register_rpc_%function_name%(%function_name%_cb_11);
    }
}
%repeat_end%

%set_repeat_start%
bool %service%ServerWrapper::reply(%return_type% rsp, int index)
{

    return %service%Server::reply(rsp, index);      
     
}
%set_repeat_end%
%namespaces_end%