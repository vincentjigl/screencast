LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CPP_INCLUDES:=$(LOCAL_PATH)/testapp $(LOCAL_PATH)/
LOCAL_C_INCLUDES:=$(LOCAL_PATH)/testapp $(LOCAL_PATH)/

LOCAL_SHARED_LIBRARIES := libewrecv
LOCAL_SHARED_LIBRARIES += libc libstagefright liblog libutils libbinder libstagefright_foundation \
						  libmedia libmedia_omx libgui libui 

LOCAL_C_INCLUDES := \
	frameworks/av/media/libstagefright \
	frameworks/native/include/media/openmax \
	bionic/libc/include/\
	
LOCAL_CFLAGS += -DANDROID -fPIC -pie -fPIE 
LOCAL_CFLAGS += -Wno-reorder
LOCAL_CFLAGS += -Wno-writable-strings
LOCAL_CFLAGS += -Wno-unused-parameter
LOCAL_CFLAGS += -Wno-unknown-pragmas
LOCAL_CFLAGS += -Wno-implicit-function-declaration

LOCAL_CXXFLAGS += -DANDROID -fPIC -pie -fPIE 
LOCAL_LDFLAGS += -pie -fPIE
 
LOCAL_SRC_FILES	:= testapp/recvdemo.cpp mcdec/mcdec.cpp
LOCAL_MODULE	:= recvdemo

APP_ALLOW_MISSING_DEPS=true
$(warning $(LOCAL_SRC_FILES))

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_CPP_INCLUDES:=$(LOCAL_PATH)/testapp $(LOCAL_PATH)/
LOCAL_C_INCLUDES:=$(LOCAL_PATH)/testapp $(LOCAL_PATH)/

LOCAL_SHARED_LIBRARIES := libewpair

LOCAL_CFLAGS += -DANDROID -fPIC -pie -fPIE 
LOCAL_CFLAGS += -Wno-unused-parameter
LOCAL_CFLAGS += -Wno-implicit-function-declaration
LOCAL_CXXFLAGS += -DANDROID -fPIC -pie -fPIE 
LOCAL_LDFLAGS += -pie -fPIE
 
LOCAL_SRC_FILES	:= testapp/pairdemo.c
LOCAL_MODULE	:= pairdemo

APP_ALLOW_MISSING_DEPS=true
$(warning $(LOCAL_SRC_FILES))

include $(BUILD_EXECUTABLE)
