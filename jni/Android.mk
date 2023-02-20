LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


UTILS_DIR=$(shell pwd)/utils
# $(warning "$(UTILS_DIR)")

UTILS_S = $(shell find $(UTILS_DIR) -name "*.c")
# $(warning "$(UTILS_S)")

UTILS_H=$(UTILS_DIR)/include
# $(warning "$(UTILS_H)")

LOCAL_C_INCLUDES := $(UTILS_H)

# build source file

BUILD_NAME=rootz

BUILD_SN=../exploit/$(RS)

LOCAL_SRC_FILES := \
  $(UTILS_S) \
  $(BUILD_SN)/rootz.c \

LOCAL_MODULE    := $(BUILD_NAME)


LOCAL_CFLAGS += -Werror -fcommon
LOCAL_LDFLAGS += -static

include $(BUILD_EXECUTABLE)
