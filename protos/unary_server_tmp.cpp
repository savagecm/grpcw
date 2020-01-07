#include "grpc/%grpc_async_header%"
%namespaces%

void %service%Server::Run(std::string host, uint16_t port)
{
        std::string server_address = host + ":" + std::to_string(port);
        ::grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);
        cq_ = builder.AddCompletionQueue();
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;
        %repeat_start%
        new %function_name%CallData(&service_, cq_.get());
        %repeat_end%
        ServerThread_ = std::thread([this](){
            HandleRpcs();            
        });   
}

%repeat_start%
std::function<void (%function_argument_type% const &, int index)> %service%Server::_%function_name%_cb;
%repeat_end%

std::map<int,void*> %service%Server::_affi_map;
std::atomic<int> %service%Server::_unique_id;
std::mutex %service%Server::mtx;
%namespaces_end%