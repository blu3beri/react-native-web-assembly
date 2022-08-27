#include <vector>
#include <include/wasmer.h>
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
void handleError(jsi::Runtime &rt, ErrorCode code) {
  if (code == 0)
    return;

  jsi::Object JSON = rt.global().getPropertyAsObject(rt, "JSON");
  jsi::Function JSONParse = JSON.getPropertyAsFunction(rt, "parse");
  jsi::Object parsedErrorObject =
      JSONParse.call(rt, "{'code': 1, 'mesage': 'Unknown error'}").getObject(rt);
  jsi::Value message = parsedErrorObject.getProperty(rt, "message");
  if (message.isString()) {
    throw jsi::JSError(rt, message.getString(rt).utf8(rt));
  }
  throw jsi::JSError(rt, "Could not get message with code: " +
                             std::to_string(code));
};

template <>
wasm_byte_vec_t jsiToValue<wasm_byte_vec_t>(jsi::Runtime &rt, jsi::Object &options,
                                  const char *name, bool optional) {
  jsi::Value value = options.getProperty(rt, name);

  if (value.isObject() && value.asObject(rt).isArrayBuffer(rt)) {
    jsi::ArrayBuffer arrayBuffer = value.getObject(rt).getArrayBuffer(rt);
    size_t size = arrayBuffer.size(rt);
    wasm_byte_t* bytes = (wasm_byte_t*)arrayBuffer.data(rt);
    return wasm_byte_vec_t {size, bytes};
  }

  if (optional)
    return wasm_byte_vec_t{0, 0};

  throw jsi::JSError(rt, errorPrefix + name + errorInfix + "Uint8Array");
}

} // namespace turboModuleUtility
