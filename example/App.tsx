import { StatusBar } from 'expo-status-bar'
import { StyleSheet, Text, View } from 'react-native'
import { readFile, MainBundlePath, readDir } from 'react-native-fs'
import WebAssembly from 'react-native-web-assembly'
import { Buffer } from 'buffer'

export default function App() {
  const file =
    'AGFzbQEAAAABBQFgAAF/AwIBAAcKAQZhbnN3ZXIAAAoGAQQAQSoL'
  const res = Buffer.from(file, 'base64')
  const module = WebAssembly.compile(res)
  console.log(module)
  console.log(module.answer(1,2))
  return (
    <View style={styles.container}>
      <Text>{WebAssembly.version}</Text>
      <StatusBar style="auto" />
    </View>
  )
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
})
