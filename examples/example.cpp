/*
 * Copyright (c) 2016-20018 Max Cong <savagecm@qq.com>
 * this code can be found at https://github.com/maxcong001
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>

#include <grpc++/grpc++.h>
#include "gen_target/greeter.pb.h"
#include "gen_target/greeter.grpc.pb.h"

#include "grpc/greeterClient.hpp"
#include "grpc/greeterServer.hpp"

using namespace helloworld;
int main(int argc, char **argv)
{
    int index_test = 0;
    set_log_level(logger_iface::log_level::debug);
    {
        __LOG(debug, "now start server");
        GreeterServer server;
        server.register_rpc_SayHello([&index_test](HelloRequest const &request, int index) {
            index_test = index;
            __LOG(debug, "[server][hello] receive request with name : " << request.name() << " index test is : " << index_test);
        });
        server.Run("127.0.0.1", 2223);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        GreeterClient greeter(grpc::CreateChannel("127.0.0.1:2223", grpc::InsecureChannelCredentials()));
        greeter.init();
        for (int i = 1; i < 10; i++)
        {
            HelloRequest request;
            request.set_name("this is a request  --> ");
            greeter.SayHello(request, [](HelloReply reply) {
                __LOG(debug, "[hello]got reply from server : " << reply.message());
            });
        }
        // give the program some time to run before delete client and server
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::string _reply_str = "HelloAgain ";
        HelloReply _relpy;
        _relpy.set_message(_reply_str);
        GreeterServerReply(_relpy, index_test);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    __LOG(error, "example exit now!");
    return 0;
}
