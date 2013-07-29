/*
 * android_jakev_Lime.cpp - JNI interface for LiME
 * Copyright (c) 2013 Jake Valletta
 *
 *
 * Author:
 * Jake Valletta     -javallet@gmail.com, @jake_valletta
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_NAMESPACE "log.tag."
#define LOG_TAG "Lime.cpp"

#include <assert.h>
#include <cutils/properties.h>
#include <utils/String8.h>
#include <utils/Log.h>
#include <jakev/lime.h>

#include "jni.h"
#include "utils/misc.h"
#include "android_runtime/AndroidRuntime.h"

namespace android {
/*
 * Determine if LiME is ready.
 */
static jboolean android_jakev_Lime_is_ready(JNIEnv* env, jobject clazz)
{
   int ret_val;
   
   ret_val = __is_ready();
   switch (ret_val) {
      case LIME_STATUS_READY:
         return JNI_TRUE;

      case LIME_STATUS_BUSY:
         return JNI_FALSE;

      default:
         LOGE("Not sure what __is_ready() returned. %d\n", ret_val);
         return JNI_FALSE;
   }
}

/* 
 * Dump memory to a file on disk.
 */ 
static jint android_jakev_Lime_dump_memory_disk(JNIEnv* env, jobject clazz, jstring file_name, jint mode, jint dio)
{
   int ret_val;
   const char *tmp = env->GetStringUTFChars(file_name, NULL);
   
   return  __dump_memory_disk(tmp, (int)mode, (int)dio);
}

/*
 * Dump memory to a specified TCP port.
 */
static jint android_jakev_Lime_dump_memory_port(JNIEnv * env, jobject clazz, jint port, jint mode, jint dio)
{
   return __dump_memory_tcp((int)port, (int)mode, (int)dio);
}

/*
 * JNI registration.
 */
static JNINativeMethod gMethods[] = {
   /* name, signature, funcPtr */
   { "is_ready",      "()Z", (void*) android_jakev_Lime_is_ready },
   { "dump_memory_disk",  "(Ljava/lang/String;II)I", (void*) android_jakev_Lime_dump_memory_disk },
   { "dump_memory_port",  "(III)I", (void*) android_jakev_Lime_dump_memory_port }, 
};

int register_android_jakev_Lime(JNIEnv* env)
{
   jclass clazz = env->FindClass("android/jakev/Lime");

   if (clazz == NULL) {
      LOGE("Can't find android/jakev/Lime");
      return JNI_FALSE;
   }
    
   return AndroidRuntime::registerNativeMethods(env, "android/jakev/Lime", gMethods, NELEM(gMethods));
}

}; // namespace android
