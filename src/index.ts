import { NativeModules } from 'react-native'
import { WebAssembly } from './WebAssembly'

const wasm = NativeModules.WebAssembly
if (!wasm.install()) {
  throw new Error('Could not set _web_assmbly on global. Turbo Module issue')
}

export default WebAssembly
