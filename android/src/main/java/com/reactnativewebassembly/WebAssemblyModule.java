package com.reactnativewebassembly;

import android.util.Log;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import com.facebook.proguard.annotations.DoNotStrip;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

@Keep
@DoNotStrip
public class WebAssemblyModule extends ReactContextBaseJavaModule {
    static {
      System.loadLibrary("reactnativewebassembly");
    }

    public static final String NAME = "WebAssembly";

    static String TAG = "WebAssembly";

    public WebAssemblyModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @Override
    @NonNull
    public String getName() {
        return TAG;
    }

    private static native void installNative(long jsiRuntimePointer, CallInvokerHolderImpl jsCallInvokerHolder);

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean install() {
      try {
        System.loadLibrary("reactnativewebassembly");
        ReactContext context = getReactApplicationContext();
        long jsContextPointer = context.getJavaScriptContextHolder().get();
        CallInvokerHolderImpl holder = (CallInvokerHolderImpl) context.getCatalystInstance().getJSCallInvokerHolder();
        installNative(jsContextPointer, holder);
        return true;
      } catch (Exception exception) {
        Log.e(NAME, "Failed to install JSI Bindings!", exception);
        return false;
      }
    }
}
