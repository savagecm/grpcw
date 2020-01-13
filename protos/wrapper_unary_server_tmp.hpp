#pragma once
#include "%generated_protobuf_header%"

%namespaces%


class %service%ServerWrapper
{
  public:
    ~%service%ServerWrapper();
    explicit %service%ServerWrapper();
    %repeat_start%
    typedef void (*%function_name%_cb)(%function_argument_type% const &, int index);
    void register_rpc_%function_name%(%function_name%_cb user_callback_fn);
    %repeat_end%
    void Run(char* host, uint16_t port);

%set_repeat_start%
    static bool reply(%return_type% rsp, int index);
%set_repeat_end%
};
%namespaces_end%