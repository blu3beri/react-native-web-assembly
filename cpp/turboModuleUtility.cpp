#include <include/wasmer.h>
#include <include/wasm.hh>
#include <turboModuleUtility.h>

namespace turboModuleUtility {

using byteVector = std::vector<uint8_t>;

std::shared_ptr<react::CallInvoker> invoker;

void registerTurboModule(jsi::Runtime &rt,
                         std::shared_ptr<react::CallInvoker> jsCallInvoker) {
  // Setting the callInvoker for async code
  invoker = jsCallInvoker;
  // Create a TurboModuleRustHostObject
  auto instance = std::make_shared<TurboModuleHostObject>(rt);
  // Create a JS equivalent object of the instance
  jsi::Object jsInstance = jsi::Object::createFromHostObject(rt, instance);
  // Register the object on global
  rt.global().setProperty(rt, "_web_assembly", std::move(jsInstance));
}

void assertValueIsObject(jsi::Runtime &rt, const jsi::Value *val) {
  val->asObject(rt);
}
void handleError(jsi::Runtime &rt) {
    int error_len = wasmer_last_error_length();
    if(error_len == 0) return;
    
    char *error_str = (char*)malloc(error_len);
    wasmer_last_error_message(error_str, error_len);
    throw jsi::JSError(rt, error_str);
    
};

template <>
wasm::vec<char> jsiToValue<wasm::vec<char>>(jsi::Runtime &rt, jsi::Object &options,
                                  const char *name, bool optional) {
  jsi::Value value = options.getProperty(rt, name);

  if (value.isObject() && value.asObject(rt).isArrayBuffer(rt)) {
    jsi::ArrayBuffer arrayBuffer = value.getObject(rt).getArrayBuffer(rt);
    char* data = (char*)arrayBuffer.data(rt);
    return wasm::vec<char>::make(std::string(data));
  }

  if (optional)
      return wasm::vec<char>::make();

  throw jsi::JSError(rt, errorPrefix + name + errorInfix + "Uint8Array");
}

} // namespace turboModuleUtility
