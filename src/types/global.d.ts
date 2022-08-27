export {}

declare global {
  const _web_assembly = {
    version: ({}) => string,
    testing: ({}) => number,
    compile: ({ wasm_bytes }: { wasm_bytes: ArrayBuffer }) => number,
  }
}
