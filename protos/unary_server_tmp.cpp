#include "grpc/%grpc_async_header%"
%namespaces%
%repeat_start%
std::function<std::future<%return_type%>(%function_argument_type% const &)> %service%Server::_%function_name%_cb;
%repeat_end%
%namespaces_end%