#pragma once
#include "%generated_protobuf_header%"
#include "%generated_grpc_header%"
#include <grpcpp/grpcpp.h>
#include "logger/logger.hpp"
#include <thread>
#include <atomic>
#include <mutex> 
%namespaces%
   #define   send_reply(rsp,  index)\
    {\
        void * tmp_this = get_from_affi_map(index);\
        if(tmp_this)\
        {\
            static_cast<CallData *>(tmp_this)->Finish(rsp);\
        }\
    }\
class %service%Server final
{
  public:
    ~%service%Server()
    {
        server_->Shutdown();
        cq_->Shutdown();
        ServerThread_.join();
    }
    struct CallData{
        virtual ~CallData(){};
        virtual void Proceed() = 0;
    };
    void Run(std::string host, uint16_t port);

    %repeat_start%
    using %function_name%_cb = std::function<%return_type%(%function_argument_type% const &)>;
    //typedef %return_type% (*%function_name%_cb)(%function_argument_type% const &)

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
                
                _%function_name%_cb(request_, get_unique_id());
                //responder_.Finish(_%function_name%_cb(request_), ::grpc::Status::OK, this);
            }
            else
            {
                delete this;
            }
        }

        void Finish(%return_type% rsp)
        {
            status_ = FINISH;
            responder_.Finish(rsp, ::grpc::Status::OK, this);
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

    static set_affi_map(int key, void* value){
        std::lock_guard<std::mutex> lck (mtx);
        _affi_map[key] = value;

    }
    static void* get_from_affi_map(int key){
        std::lock_guard<std::mutex> lck (mtx);
        void *ret = NULL;
        try
        {
            ret =  _affi_map.at(key);
        }catch(...)
        {
            ret = NULL;
        }
        return ret;
    }
    static int get_unique_id()
    {
        return _unique_id++;
    }
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
    static std::map<int,void*> _affi_map;
    static atomic<int> _unique_id;
    static std::mutex mtx;
};
%namespaces_end%
