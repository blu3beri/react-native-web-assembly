#include <webAssembly.h>
#include "include/wasmer.h"

using namespace turboModuleUtility;

namespace webAssembly {

jsi::Value version(jsi::Runtime &rt, jsi::Object options) {
  const char* wasmerVersion = wasmer_version();
  return jsi::String::createFromAscii(rt, wasmerVersion);
};

}
