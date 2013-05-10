// https://github.com/agnat/node_mdns/blob/6dbd4619c/src/socket_watcher.hpp
// Copyright (c) 2012 Toby Ealden

#include <node.h>
#include <node_version.h>

class SocketWatcher : public node::ObjectWrap {
    public:
        SocketWatcher();

        static void Initialize(v8::Handle<v8::Object> target);

    private:
        uv_poll_t* poll_;
        int fd_;
        int events_;

        static v8::Handle<v8::Value> New(const v8::Arguments & args);
        static v8::Handle<v8::Value> Set(const v8::Arguments & args);
        static v8::Handle<v8::Value> Start(const v8::Arguments& args);
        static v8::Handle<v8::Value> Stop(const v8::Arguments& args);
        
        void Start();
        void Stop();
        static void Callback(uv_poll_t *w, int status, int events);
};
