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

#include "grpcwrapper/grpc/greeterClientWrapper.hpp"
#include "grpcwrapper/grpc/greeterServerWrapper.hpp"

#include <unistd.h>

using namespace helloworld;

void clientFunction(HelloReply reply)
{
    std::cout << "got reply from server : " << reply.message() << std::endl;
}
HelloReply serverFunction(HelloRequest const &request)
{
    std::cout << "receive request with name : " << request.name() << std::endl;
    std::string _reply_str = "Hello " + request.name();
    HelloReply _relpy;
    _relpy.set_message(_reply_str);
    return _relpy;
}

int main(int argc, char **argv)
{

    std::cout << "now start server" << std::endl;
    GreeterServerWrapper server;
    server.register_rpc_SayHello(serverFunction);
    server.Run("127.0.0.1", 2221);

    sleep(2);
    std::cout << "now start client" << std::endl;

    GreeterClientWrapper greeter;
    greeter.init("127.0.0.1:2221");

    for (int i = 0; i < 10; i++)
    {
        HelloRequest request;
        request.set_name("this is a request  --> ");
        greeter.SayHello(request, clientFunction);
    }
    sleep(2);
    std::cout << "exit example" << std::endl;
    return 0;
}
