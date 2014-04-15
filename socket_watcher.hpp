// Copyright (c) 2012 Toby Ealden.
// Copyright (c) 2014 Martin Man.
// vim: ts=2 sw=2 et

#include <node.h>
#include <node_version.h>
#include <node_object_wrap.h>
#include <uv.h>

class SocketWatcher : public node::ObjectWrap
{
  public:
    SocketWatcher();

    static void Initialize(v8::Handle<v8::Object> exports);

  private:
    uv_poll_t* poll_;
    int fd_;
    int events_;

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);

    void StartInternal();
    void StopInternal();
    static void Callback(uv_poll_t *w, int status, int events);
};
