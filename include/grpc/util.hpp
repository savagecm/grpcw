#pragma once

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <future>
#include <iostream>
#include <thread>
#include <list>
#include "logger/logger.hpp"
enum class State
{
    DISCONNECTED,
    CONNECTED,
    CONNECTING,
    FAULTY
};

template <typename Service>
struct Connection
{
    std::string ip;

    std::string server_address;
    std::shared_ptr<::grpc::Channel> channel;
    std::shared_ptr<typename Service::Stub> stub;
    ::grpc::CompletionQueue cq;

    std::thread listen_thread;
    bool run;

    bool internal_faulty_flag;
    State state;
    uint16_t port;
    Connection() : run(true), internal_faulty_flag(false), state(State::DISCONNECTED), port(0)
    {
    }
    ~Connection()
    {
        __LOG(warn, "dtor of Connection is called");
        cq.Shutdown();
        if (listen_thread.joinable())
        {
            __LOG(warn,"join the thread with id :" << listen_thread.get_id());
            listen_thread.join();
        }
    }
};

struct Call_Handler : std::enable_shared_from_this<Call_Handler>
{
    virtual ~Call_Handler() = default;
    virtual void Proceed() = 0;
};
