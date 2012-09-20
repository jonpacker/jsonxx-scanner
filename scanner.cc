#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include <jsonxx/scanner.hpp>
#include <jsonxx/scanner_listener.hpp>

namespace jsonxxbind {

  class scanner_fireall : public jsonxx::scanner_listener {
  public:
    scanner_fireall(const v8::Handle<v8::Object> &jsObj) 
      : jsonxx:scanner_listener() , jsObj(jsObj) {};

    void emit(int argc, v8::Handle<v8::Value> argv[]) {
      v8::Handle<v8::Function> emit = jsObj->Get(v8::String::New("emit"));
      emit->Call(jsObj, argc, argv);
    };

    void fire_event(const std::string& event_name, 
                    const std::string& event_data) {
      v8::HandleScope scope;
      
      v8::Handle<Value> argv[] = {
        v8::String::New(event_name.data()),
        v8::String::New(event_data.data())
      };

      emit->Call(jsObj, 2, argv);
      scope->Close(v8::Undefined());
    };

    void fire_event(const std::string& event_name) {
      v8::HandleScope scope;
      
      v8::Handle<Value> argv[] = { v8::String::New(event_name.data()) };

      emit->Call(jsObj, 1, argv);
      scope->Close(v8::Undefined());
    };

    virtual void number(const std::string& num) {
      fire_event(std::string("number"), num);
    };

  private:
    const Handle<Object> &jsObj;
  };
  
  class scanner_iface : public node::ObjectWrap {
    scanner_iface(jsonxx::scanner_listener& listener) 
      : ObjectWrap(), scanner(new jsonxx::scanner(listener)) {};
  };

}
