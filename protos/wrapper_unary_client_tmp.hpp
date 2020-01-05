#pragma once
#include "%generated_protobuf_header%"

%namespaces%
class %service%ClientWrapper
{
  public:
    ~%service%ClientWrapper();
    %repeat_start%
    void %function_name%(%function_argument_type% const & request, void(*cb)(%return_type%));
    %repeat_end%
    void init(char* ipPort, int secureType = 0);
};
%namespaces_end%
