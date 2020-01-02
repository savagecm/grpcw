#pragma once
#include "%generated_protobuf_header%"
#include "%generated_grpc_header%"
#include "grpc/util.hpp"
%namespaces%
class %service%Server final
{
  public:
    ~%service%Server()
    {
        server_->Shutdown();
        cq_->Shutdown();
        ServerThread_.join();
    }
    void Run(std::string host, uint16_t port)
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
    struct CallData{
        virtual ~CallData(){};
        virtual void Proceed() = 0;
    };
    %repeat_start%
    using %function_name%_cb = std::function<std::future<%return_type%>(%function_argument_type% const &)>;
    static void register_rpc_%function_name%(%function_name%_cb user_callback_fn)
    {  
        _%function_name%_cb = user_callback_fn;
    }
    static %function_name%_cb _%function_name%_cb;
    class %function_name%CallData : public CallData
    {
      public:
        %function_name%CallData(%service%::AsyncService *service, ::grpc::ServerCompletionQueue *cq)
            : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE)
        {
            Proceed();
        }
        void Proceed()
        {
            if (status_ == CREATE)
            {
                status_ = PROCESS;
                service_->Request%function_name%(&ctx_, &request_, &responder_, cq_, cq_, this);
            }
            else if (status_ == PROCESS)
            {
                new %function_name%CallData(service_, cq_);
                auto reply_futher = _%function_name%_cb(request_);
                status_ = FINISH;
                responder_.Finish(reply_futher.get(), ::grpc::Status::OK, this);
            }
            else
            {
                delete this;
            }
        }
      private:
        %service%::AsyncService *service_;
        ::grpc::ServerCompletionQueue *cq_;
        ::grpc::ServerContext ctx_;
        %function_argument_type% request_;
        %return_type% reply_;
        ::grpc::ServerAsyncResponseWriter<%return_type%> responder_;
        enum CallStatus
        {
            CREATE,
            PROCESS,
            FINISH
        };
        CallStatus status_;
    };
    %repeat_end%   
  private:
    void HandleRpcs()
    {
        void * got_tag(nullptr);
        bool ok = false;
        while (true)
        {
            __LOG(debug, "wait for message!");
            auto tmp_status = cq_->AsyncNext(&got_tag, &ok, std::chrono::system_clock::now() + std::chrono::seconds(1));
            if (::grpc::CompletionQueue::GOT_EVENT == tmp_status)
            {
                __LOG(debug, "got message form client");
                if (ok)
                {
                    static_cast<CallData *>(got_tag)->Proceed();
                }
                else
                {
                    __LOG(warn, "cq Next retur not ok");   
                }
            }
            else if (::grpc::CompletionQueue::SHUTDOWN == tmp_status)
            {
                __LOG(warn, "receive shutdown message! exit now!");
                return;
            }
            else
            {
                __LOG(debug, "receive message timeout");
            }         
        }   

    }
    std::unique_ptr<::grpc::ServerCompletionQueue> cq_;
    %service%::AsyncService service_;
    std::unique_ptr<::grpc::Server> server_;
    std::string host;
    uint16_t port;
    std::thread ServerThread_;
};
%namespaces_end%
