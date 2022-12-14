// https://github.com/microsoft/TypeScript/blob/15f7b6f68791487c059d41d5fe99117f79e1f1a2/lib/lib.dom.d.ts#L17759
export class WebAssembly {
  static get version() {
    return _web_assembly.version({})
  }
  static testing() {
    return _web_assembly.testing({})
  }
  static compile(bytes: Uint8Array) {
    return _web_assembly.compile({ wasm_bytes: bytes.buffer })
  }
}
