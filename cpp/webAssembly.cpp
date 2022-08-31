#include <webAssembly.h>
#include "include/wasmer.h"
#include "include/wasm.hh"

using namespace turboModuleUtility;
using namespace wasm;

namespace webAssembly {

jsi::Value version(jsi::Runtime &rt, jsi::Object options) {
  const char* wasmerVersion = wasmer_version();
  return jsi::String::createFromAscii(rt, wasmerVersion);
};

jsi::Value compile(jsi::Runtime &rt, jsi::Object options) {
  const vec<char> wasm_bytes = jsiToValue<vec<char>>(rt, options, "wasm_bytes");
    
  own<Engine> engine = Engine::make();
  own<Store> store = Store::make(engine.get());
  own<Module> module = Module::make(store.get(), wasm_bytes);

  if (!module) {
    handleError(rt);
  }
    
  const vec<Extern*> import_object = vec<Extern*>::make();
  own<Instance> instance = Instance::make(store.get(), module.get(), import_object);
    
  if (!instance) {
    handleError(rt);
  }
    
  // TODO: are these in sync?
  ownvec<Extern> exports = instance.get()->exports();
  ownvec<ExportType>exportTypes = module.get()->exports();
      
    
    jsi::Object object = jsi::Object(rt);
    
    for(int i = 0; i < exports.size(); i++) {
        wasm_func_t* func = wasm_extern_as_func(exports.data[i]);
        wasm_exporttype_t* export_type = export_types.data[i];

        wasm_functype_t* type = wasm_func_type(func);
        const wasm_name_t* export_name = wasm_exporttype_name(export_type);
        const char* name = (const char*)export_name->data;

        auto funcie = jsi::Function::createFromHostFunction(
                  rt,
                  jsi::PropNameID::forAscii(rt, name),
                  1,
                  [func](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) {
                    // TODO:should be count
                    wasm_val_t args_val[0];
                    // TODO: 1. extract the arguments from args
                    //       2. convert them to WASMER arguments
                    //       3. call the function
                    //       4. jsi-ify the return value

                    // TODO: we need to know which arguments are needed by the wasm function
                    for(int j = 0; j<count;j++) {
                      if(!args[j].isNumber()) {
                        throw jsi::JSError(rt, "only number");
                      }
                    }

                    // TODO: for now we assume a single return value (maybe we can use wasmer to detect these things)
                    wasm_val_t results_val[1] = { WASM_INIT_VAL };
                    wasm_val_vec_t wasm_args = WASM_ARRAY_VEC(args_val);
                    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

                    wasm_func_call(func, &wasm_args, &results);
                    auto ret = results_val[0].of.i32;
                    return jsi::Value(ret);
        });
        object.setProperty(rt, name, funcie);
    }
    
  return object;
}

jsi::Value testing(jsi::Runtime &rt, jsi::Object options) {
  const char *wat_string =
          "(module\n"
          "  (type $sum_t (func (param i32 i32) (result i32)))\n"
          "  (func $sum_f (type $sum_t) (param $x i32) (param $y i32) (result i32)\n"
          "    local.get $x\n"
          "    local.get $y\n"
          "    i32.add)\n"
          "  (export \"sum\" (func $sum_f)))";

    wasm_byte_vec_t wat;
    wasm_byte_vec_new(&wat, strlen(wat_string), wat_string);
    wasm_byte_vec_t wasm_bytes;
    wat2wasm(&wat, &wasm_bytes);
    wasm_byte_vec_delete(&wat);

    printf("Creating the store...\n");
    wasm_engine_t* engine = wasm_engine_new();
    wasm_store_t* store = wasm_store_new(engine);

    printf("Compiling module...\n");
    wasm_module_t* module = wasm_module_new(store, &wasm_bytes);

    if (!module) {
        printf("> Error compiling module!\n");
    }

    wasm_byte_vec_delete(&wasm_bytes);

    printf("Creating imports...\n");
    wasm_extern_vec_t import_object = WASM_EMPTY_VEC;

    printf("Instantiating module...\n");
    wasm_instance_t* instance = wasm_instance_new(store, module, &import_object, NULL);

    if (!instance) {
      printf("> Error instantiating module!\n");
    }

    printf("Retrieving exports...\n");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);

    if (exports.size == 0) {
        printf("> Error accessing exports!\n");
    }

    printf("Retrieving the `sum` function...\n");
    wasm_func_t* sum_func = wasm_extern_as_func(exports.data[0]);

    if (sum_func == NULL) {
        printf("> Failed to get the `sum` function!\n");
    }

    printf("Calling `sum` function...\n");
    wasm_val_t args_val[2] = { WASM_I32_VAL(3), WASM_I32_VAL(4) };
    wasm_val_t results_val[1] = { WASM_INIT_VAL };
    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

    if (wasm_func_call(sum_func, &args, &results)) {
        printf("> Error calling the `sum` function!\n");

    }

    printf("Results of `sum`: %d\n", results_val[0].of.i32);

    wasm_module_delete(module);
    wasm_extern_vec_delete(&exports);
    wasm_instance_delete(instance);
    wasm_store_delete(store);
    wasm_engine_delete(engine);

    return jsi::Value((int)results_val[0].of.i32);
}

}
