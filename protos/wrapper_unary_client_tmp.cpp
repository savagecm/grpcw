#include "%generated_protobuf_header%"
#include "%generated_grpc_header%" 
#include "grpc/%grpc_async_header%"
#include "grpc/%grpc_async_wrapper_header%"

#include <grpcpp/grpcpp.h>
#include <memory>

#include "logger/logger.hpp"
%namespaces%
std::unique_ptr<%service%Client> %service%ClientWrapper_uptr;


%service%ClientWrapper::~%service%ClientWrapper()
{
    if(%service%ClientWrapper_uptr)
    {
        %service%ClientWrapper_uptr.reset(nullptr);
    }
}



void %service%ClientWrapper::init(char* ipPort, int secureType)
{
    std::string ipPortString(ipPort);
    if(secureType == 0){
        std::unique_ptr<%service%Client> used_client_uptr(new %service%Client(grpc::CreateChannel(ipPortString, grpc::InsecureChannelCredentials())));
        %service%ClientWrapper_uptr = std::move(used_client_uptr);
        %service%ClientWrapper_uptr->init();
    }
    // else todo

}


 
%repeat_start%
void %service%ClientWrapper::%function_name%(%function_argument_type% const & request, void(*cb)(%return_type%))
{
    if(%service%ClientWrapper_uptr)
    {
        %service%ClientWrapper_uptr->%function_name%(request, cb);
    }
    else
    {
        /* error log */
    }       
}
%repeat_end%
%namespaces_end%