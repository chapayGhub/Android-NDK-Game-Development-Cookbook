#
LOCAL_PATH := $(call my-dir)

# bringing in libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenAL
LOCAL_SRC_FILES :=../../../Libs.Android/libOpenAL.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := App8

# using Engine include folders
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../src \
	$(LOCAL_PATH)/../src/core \
	$(LOCAL_PATH)/../src/fs \
	$(LOCAL_PATH)/../src/sound \
	$(LOCAL_PATH)/../src/threading \

LOCAL_SRC_FILES += Wrappers.cpp WrappersJNI.c \
	../src/sound/LAL.cpp \
	../main.cpp \
	../src/threading/Thread.cpp \
	../src/threading/tinythread.cpp \
	../src/core/iIntrusivePtr.cpp \
	../src/fs/FileSystem.cpp \
	../src/fs/libcompress.c \
	../src/fs/Archive.cpp \
	../src/Rendering.cpp

LOCAL_ARM_MODE := arm
PROJ_CFLAGS := -Wno-unused-value -Wno-format-extra-args -Wno-format-security -O3 -Werror -DANDROID -DDISABLE_IMPORTGL -D_DISABLE_TUNNELLERS_ -D_DISABLE_METHODS_ -DDISABLE_GUARD_UNGUARD_MECHANISM -isystem $(SYSROOT)/usr/include/

ifeq ($(TARGET_ARCH),x86)
	GLOBAL_CFLAGS  := $(PROJ_CFLAGS)
else
	GLOBAL_CFLAGS  := -mfpu=vfp -mfloat-abi=softfp -fno-short-enums $(PROJ_CFLAGS)
endif

LOCAL_CFLAGS := $(GLOBAL_CFLAGS)

LOCAL_CPPFLAGS += -std=gnu++0x

LOCAL_LDLIBS    += -llog -lGLESv2
LOCAL_STATIC_LIBRARIES += OpenAL

include $(BUILD_SHARED_LIBRARY)
