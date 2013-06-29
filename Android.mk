LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := uv_static

LOCAL_MODULE_FILENAME := libuv

LOCAL_SRC_FILES := \
src/fs-poll.c \
src/inet.c \
src/uv-common.c \
src/version.c \
src/unix/linux-core.c \
src/unix/linux-inotify.c \
src/unix/linux-syscalls.c \
src/unix/pthread-fixes.c \
src/unix/async.c \
src/unix/core.c \
src/unix/dl.c \
src/unix/error.c \
src/unix/fs.c \
src/unix/getaddrinfo.c \
src/unix/loop.c \
src/unix/loop-watcher.c \
src/unix/pipe.c \
src/unix/poll.c \
src/unix/process.c \
src/unix/signal.c \
src/unix/stream.c \
src/unix/tcp.c \
src/unix/thread.c \
src/unix/threadpool.c \
src/unix/timer.c \
src/unix/tty.c \
src/unix/udp.c

ifdef APP_PLATFORM_LESS_9
     LOCAL_SRC_FILES += \
src/unix/android/pthread-rwlock.c  \
src/unix/android/fdatasync.c \
src/unix/android/sysinfo.c \
src/unix/android/stime.c 

endif

LOCAL_EXPORT_C_INCLUDES :=$(LOCAL_PATH)/include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
                    $(LOCAL_PATH)/include \
					$(LOCAL_PATH)/include/uv-private \
                    $(LOCAL_PATH)/src \
					$(LOCAL_PATH)/src/unix

LOCAL_LDLIBS := -ldl \
                -lrt 

include $(BUILD_STATIC_LIBRARY)
