LOCAL_PATH := $(call my-dir)

#include $(LOCAL_PATH)/../../../Android.mk

include $(CLEAR_VARS)

LOCAL_MODULE := test-uv
LOCAL_SRC_FILES := test.cpp
LOCAL_STATIC_LIBRARIES += uv_static
include $(BUILD_SHARED_LIBRARY)

$(call import-module,libuv)