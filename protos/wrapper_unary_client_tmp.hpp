#pragma once
#include "%generated_protobuf_header%"

#include "logger/logger.hpp"
%namespaces%
class %service%ClientWrapper
{
  public:
    ~%service%ClientWrapper();
    %repeat_start%
    void %function_name%(%function_argument_type% const & request, void(*cb)(%return_type%));
    %repeat_end%
    void init(std::string ipPort, int secureType = 0);
};
%namespaces_end%
