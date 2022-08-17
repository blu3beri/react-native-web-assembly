const WebAssembly = require('./NativeWebAssembly').default

export function multiply(a: number, b: number): number {
  return WebAssembly.multiply(a, b);
}
