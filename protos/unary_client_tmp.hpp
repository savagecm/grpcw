#pragma once
#include "%generated_protobuf_header%"
#include "%generated_grpc_header%" 

#include <grpcpp/grpcpp.h>
#include "logger/logger.hpp"
%namespaces%
class %service%Client
{
  public:
    explicit %service%Client(std::shared_ptr<::grpc::Channel> channel)
        : stub_(%service%::NewStub(channel)) 
        {
            run_=true;
            channel_ = channel;
        }
    ~%service%Client()
    {
        run_=false;
        cq_.Shutdown();
        listen_thread_.join();  
        __LOG(warn, "dtor of %service%Client is called");        
    }
    %repeat_start%
    void %function_name%(%function_argument_type% const & request, void(*cb)(%return_type%))
    {
        RpcCallData<%return_type%> *call = new RpcCallData<%return_type%>();
        call->set_meta_data((void*)(cb));
        call->response_reader = stub_->Async%function_name%(&call->context, request, &cq_);
        call->response_reader->Finish(&call->reply, &call->status, (void *)call);           
    }
    %repeat_end%
    void init()
    {
        AsyncCompleteRpc();
    }
    void AsyncCompleteRpc()
    {
        listen_thread_ = std::thread(
            [this]()
            {
                void * got_tag(nullptr);
                bool ok = false;
                while (run_)
                {
                    if (nullptr == channel_.get())
                    {
                        __LOG(warn, "channel is not ready!");
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                    else
                    {
                        __LOG(debug, "wait for message!");
                        auto tmp_status = cq_.AsyncNext(&got_tag, &ok, std::chrono::system_clock::now() + std::chrono::seconds(1));
                        if (::grpc::CompletionQueue::GOT_EVENT == tmp_status)
                        {
                            __LOG(debug, "got message form server");
                            AsyncClientCall *call = static_cast<AsyncClientCall *>(got_tag);
                            if (ok)
                            {
                                if (call->status.ok())
                                {
                                    call->process();
                                }
                                else
                                {
                                    __LOG(warn, "RPC failed");
                                }
                            }
                            else
                            {
                                __LOG(warn, "cq Next retur not ok");   
                            }
                            delete call;
                        }
                        else if (::grpc::CompletionQueue::SHUTDOWN == tmp_status)
                        {
                            __LOG(warn, "receive shutdown message! exit now!");
                            run_ = false;
                            continue;
                        }
                        else
                        {
                            __LOG(debug, "receive message timeout, status is : " + tmp_status);
                        }
                    }
                }   
                __LOG(warn, "client listener thread exit!"); 
            });
    }
  private:
    struct AsyncClientCall
    {
        ::grpc::Status status;
        virtual void set_meta_data(void* data) = 0;
        virtual void process() = 0;
        virtual ~AsyncClientCall(){}
    };
    template <typename Reply>
    class RpcCallData: public AsyncClientCall
    {
        public:
        typedef void(*user_callback_func)(Reply reply);
        Reply reply;
        ::grpc::ClientContext context;
        std::unique_ptr<::grpc::ClientAsyncResponseReader<Reply>> response_reader;
        void set_meta_data(void* data) override
        {
            _usr_func=((user_callback_func) data);
        }
        void process()
        {
            _usr_func(reply);
        }
        user_callback_func _usr_func;
    };
    std::unique_ptr<%service%::Stub> stub_;
    ::grpc::CompletionQueue cq_;
    bool run_;
    std::shared_ptr<::grpc::Channel> channel_;
    std::thread listen_thread_;
};
%namespaces_end%
