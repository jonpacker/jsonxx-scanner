#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>
#include <jsonxx/scanner.hpp>
#include <jsonxx/scanner_listener.hpp>

namespace jsonxxbind {

  class scanner_fireall : public jsonxx::scanner_listener {
  public:
    scanner_fireall(const v8::Handle<v8::Object> &js_obj) 
      : jsonxx::scanner_listener() , js_obj(js_obj) {}

    void emit(int argc, v8::Handle<v8::Value> argv[]) {
      v8::Handle<v8::Function> emit = 
        v8::Handle<v8::Function>::Cast(js_obj->Get(v8::String::New("emit")));
      emit->Call(js_obj, argc, argv);
    }

    void fire_event(const std::string& event_name, 
                    const std::string& event_data) {
      v8::HandleScope scope;
      
      v8::Handle<v8::Value> argv[] = {
        v8::String::New(event_name.data()),
        v8::String::New(event_data.data())
      };

      emit(2, argv);
      scope.Close(v8::Undefined());
    }

    void fire_event(const std::string& event_name) {
      v8::HandleScope scope;
      
      v8::Handle<v8::Value> argv[] = { v8::String::New(event_name.data()) };

      emit(1, argv);
      scope.Close(v8::Undefined());
    }

    void number(const std::string& num) {
      fire_event(std::string("number"), num);
    }

    void start_object() { }
    void end_object() { }

    void start_array() { }
    void end_array() { }

    void comma() { }
    void colon() { }

    void string(const std::string&) { }
    void bool_true() { }
    void bool_false() { }
    void null() { }

  private:
    const v8::Handle<v8::Object> &js_obj;
  };
  
  class scanner_iface : public node::ObjectWrap {
  public:
    scanner_iface(v8::Handle<v8::Object> &js_parent) 
      : ObjectWrap(), 
        listener(new scanner_fireall(js_parent)),
        scanner(new jsonxx::scanner(*listener)) {}

    ~scanner_iface() {
      delete scanner;
      delete listener;
    }

    static v8::Handle<v8::Value> New(const v8::Arguments &args) {
      v8::Handle<v8::Object> holder = args.Holder();
      scanner_iface *wrapper = new scanner_iface(holder);
      wrapper->Wrap(holder);
      return holder;
    }

    static v8::Handle<v8::Value> scan_bytes(const v8::Arguments &args) {
      v8::Handle<v8::Object> buffer = v8::Handle<v8::Object>::Cast(args[0]);
      v8::Handle<v8::Object> self = args.Holder();
      scanner_iface* scwrap = node::ObjectWrap::Unwrap<scanner_iface>(self);

      if (buffer->IsUndefined() || !node::Buffer::HasInstance(buffer)) {
        return v8::ThrowException(v8::String::New("expected some dataz"));
      }

      char* buffer_contents = node::Buffer::Data(buffer);
      int buffer_size = node::Buffer::Length(buffer);

      scwrap->scanner->scan(buffer_contents, buffer_contents + buffer_size);
      return v8::Undefined();
    }
  private:
    jsonxx::scanner_listener *listener;
    jsonxx::scanner *scanner;
  };

  extern "C" {
    static void init(v8::Handle<v8::Object> target) {
      v8::HandleScope scope;
      v8::Handle<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(scanner_iface::New);
      
      NODE_SET_PROTOTYPE_METHOD(tmpl, "scan_bytes", scanner_iface::scan_bytes);

      target->Set(v8::String::NewSymbol("jxx"), tmpl->GetFunction());
    }
    NODE_MODULE(jsonxx_scanner, init);
  }
}
