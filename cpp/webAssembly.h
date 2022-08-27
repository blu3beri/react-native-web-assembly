#pragma once

#include <jsi/jsi.h>

#include <turboModuleUtility.h>

using namespace facebook;

namespace webAssembly {

jsi::Value version(jsi::Runtime &rt, jsi::Object options);

}
