// Copyright (c) 2012 Toby Ealden.
// Copyright (c) 2014 Martin Man.
// vim: ts=2 sw=2 et

#include "socket_watcher.hpp"
#include <string.h>

using namespace v8;

#if NODE_VERSION_AT_LEAST(0, 7, 8)
// Nothing
#else
namespace node
{
  Handle<Value> MakeCallback(Isolate *isolate, const Handle<Object> object, const Handle<Function> callback, int argc, Handle<Value> argv[])
  { 
    HandleScope scope(isolate);

    // TODO Hook for long stack traces to be made here.

    TryCatch try_catch;

    Local<Value> ret = callback->Call(object, argc, argv);

    if (try_catch.HasCaught()) {
      FatalException(try_catch);
      return Undefined();
    }

    return scope.Escape(ret);
  }
}  // namespace node
#endif


Persistent<String> callback_symbol;
Persistent<Function> constructor;

// mman, why is this here?
// Handle<Value> Calleback(const Arguments& args) {
//     return Undefined();
// };

SocketWatcher::SocketWatcher() : poll_(NULL), fd_(0), events_(0)
{
}

void SocketWatcher::Initialize(Handle<Object> exports)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<FunctionTemplate> t = FunctionTemplate::New(isolate, New);

  t->SetClassName(String::NewFromUtf8(isolate, "SocketWatcher"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "set", SocketWatcher::Set);
  NODE_SET_PROTOTYPE_METHOD(t, "start", SocketWatcher::Start);
  NODE_SET_PROTOTYPE_METHOD(t, "stop", SocketWatcher::Stop);

  exports->Set(String::NewFromUtf8(isolate, "SocketWatcher"), t->GetFunction());
  constructor.Reset(isolate, t->GetFunction());

  Local<String> c = String::NewFromUtf8(isolate, "callback");
  callback_symbol.Reset(isolate, c);
}

void SocketWatcher::Start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  SocketWatcher *watcher = ObjectWrap::Unwrap<SocketWatcher>(args.This());
  watcher->StartInternal();
}

void SocketWatcher::StartInternal()
{
  if (poll_ == NULL) {
    poll_ = new uv_poll_t;
    memset(poll_,0,sizeof(uv_poll_t));
    poll_->data = this;
    uv_poll_init_socket(uv_default_loop(), poll_, fd_);

    Ref();
  }

  if (!uv_is_active((uv_handle_t*)poll_)) {
    uv_poll_start(poll_, events_, &SocketWatcher::Callback);
  }
}

void SocketWatcher::Callback(uv_poll_t *w, int status, int revents)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  SocketWatcher *watcher = static_cast<SocketWatcher*>(w->data);
  assert(w == watcher->poll_);

  Local<String> symbol = Local<String>::New(isolate, callback_symbol);
  Local<Value> callback_v = watcher->handle()->Get(symbol);
  if(!callback_v->IsFunction()) {
    watcher->StopInternal();
    return;
  }

  Local<Function> callback = Local<Function>::Cast(callback_v);

  Local<Value> argv[2];
  argv[0] = Local<Value>::New(isolate, revents & UV_READABLE ? True(isolate) : False(isolate));
  argv[1] = Local<Value>::New(isolate, revents & UV_WRITABLE ? True(isolate) : False(isolate));

  node::MakeCallback(isolate, watcher->handle(), callback, 2, argv);
}

void SocketWatcher::Stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  SocketWatcher *watcher = ObjectWrap::Unwrap<SocketWatcher>(args.This());
  watcher->StopInternal();
}

void SocketWatcher::StopInternal() {
  if (poll_ != NULL) {
    uv_poll_stop(poll_);
    Unref();
  }
}

void SocketWatcher::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  SocketWatcher *s = new SocketWatcher();
  s->Wrap(args.This());
  args.GetReturnValue().Set(args.This());
}

void SocketWatcher::Set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  SocketWatcher *watcher = ObjectWrap::Unwrap<SocketWatcher>(args.This());

  if(!args[0]->IsInt32()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "First arg should be a file descriptor.")));
    return;
  }
  int fd = args[0]->Int32Value();

  int events = 0;
  if(!args[1]->IsBoolean()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Second arg should boolean (readable).")));
    return;
  }
  if(args[1]->IsTrue()) events |= UV_READABLE;

  if(!args[2]->IsBoolean()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Third arg should boolean (writable).")));
    return;
  }
  if (args[2]->IsTrue()) events |= UV_WRITABLE;

  assert(watcher->poll_ == NULL);

  watcher->fd_ = fd;
  watcher->events_ = events;
}


void Init(Handle<Object> exports)
{
  SocketWatcher::Initialize(exports);
}

NODE_MODULE(socketwatcher, Init)
