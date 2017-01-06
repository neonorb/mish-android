#include <jni.h>

#include <android/log.h>

#define APPNAME "mish-android-native"

#include <mish.h>

using namespace mish;
using namespace feta;

String message = "";

Value *printlnFunction(List<Value *> *arguments) {
    message = ((StringValue *) arguments->get(0))->value;

    return NULL;
}

List<Function *> syscalls;

void registerSyscalls() {
    // println
    List<ValueType> *printlnParameterTypes = new List<ValueType>();
    printlnParameterTypes->add(ValueType::STRING);
    Function *println = new Function("__println"_H, printlnParameterTypes,
            ValueType::VOID, BIND_FREE_CB(printlnFunction));
    syscalls.add(println);
    mish_syscalls.add(println);
}

void unregisterSyscalls() {
    Iterator<Function *> iterator = syscalls.iterator();
    while (iterator.hasNext()) {
        Function *function = iterator.next();
        mish_syscalls.remove(function);
        delete function;
    }

    syscalls.clear();
}

namespace feta {

    void crash(String message) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, message, 1);
        unregisterSyscalls();
        //std::terminate();
    }

    void log(String message) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, message, 1);
    }

    void fault(String message) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, message, 1);
    }

    void debugPrint(String message) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, message, 1);
    }

}

namespace fetaimpl {

    void *malloc(feta::size size) {
        return ::malloc(size);
    }

    void free(void *thing) {
        ::free(thing);
    }

}

extern "C"
jstring
Java_com_neonorb_mish_1android_MainActivity_stringFromJNI(JNIEnv *env, jobject) {
    registerSyscalls();

    Code *code = compile::compile("__println('hello from mish blaw blaw blaw')");
    if (code != NULL) {
        execute::execute(code);
        delete code;
    }

    unregisterSyscalls();

    return env->NewStringUTF(message);
}