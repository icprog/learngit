LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libbf_tac

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(LOCAL_PATH)/../bf_tac_tee.c $(LOCAL_PATH)/tee_adapter.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include 
LOCAL_LDLIBS :=-llog

LOCAL_SHARED_LIBRARIES := libutils \
	                      libcutils \
	                      libbf_core \
	                      liblog 

LOCAL_CERTIFICATE := platform
include $(BUILD_SHARED_LIBRARY)
