ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES:= lime.c
LOCAL_MODULE := lime
LOCAL_STATIC_LIBRARIES := liblime liblog

include $(BUILD_EXECUTABLE)

endif  # TARGET_SIMULATOR != true

