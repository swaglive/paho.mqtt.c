LOCAL_PATH := $(call my-dir)
APP_ALLOW_MISSING_DEPS = true
build_path := ../build
libpaho-mqtt3_lib_path := ../src
libpaho-mqtt-helper_lib_path := $(libpaho-mqtt3_lib_path)/asyncBridgeHelpers
libpaho-mqtt3_c_includes := $(LOCAL_PATH)/$(libpaho-mqtt3_lib_path) \
	external/hdc/android-ifaddrs \
	external/openssl/include \
	external/zlib \
	$(LOCAL_PATH)/$(build_path) \

openssl_prebuild_dir = ../Prebuilt-OpenSSL-Android/Prebuilt
armeabi-v7a_openssl_prebuild_name = armv7-shared
arm64-v8a_openssl_prebuild_name = arm64-shared
x86_openssl_prebuild_name = x86-shared

MAJOR_VERSION := $(shell cat $(LOCAL_PATH)/../version.major)
MINOR_VERSION := $(shell cat $(LOCAL_PATH)/../version.minor)
PATCH_VERSION := $(shell cat $(LOCAL_PATH)/../version.patch)
VERSION := $(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)

libpaho-mqtt3_local_src_c_files_common := \
	$(libpaho-mqtt3_lib_path)/MQTTProtocolClient.c \
	$(libpaho-mqtt3_lib_path)/Tree.c \
	$(libpaho-mqtt3_lib_path)/Heap.c \
	$(libpaho-mqtt3_lib_path)/MQTTPacket.c \
	$(libpaho-mqtt3_lib_path)/Clients.c \
	$(libpaho-mqtt3_lib_path)/Thread.c \
	$(libpaho-mqtt3_lib_path)/utf-8.c \
	$(libpaho-mqtt3_lib_path)/StackTrace.c \
	$(libpaho-mqtt3_lib_path)/MQTTProtocolOut.c \
	$(libpaho-mqtt3_lib_path)/Socket.c \
	$(libpaho-mqtt3_lib_path)/Log.c \
	$(libpaho-mqtt3_lib_path)/Messages.c \
	$(libpaho-mqtt3_lib_path)/LinkedList.c \
	$(libpaho-mqtt3_lib_path)/MQTTPersistence.c \
	$(libpaho-mqtt3_lib_path)/MQTTPacketOut.c \
	$(libpaho-mqtt3_lib_path)/SocketBuffer.c \
	$(libpaho-mqtt3_lib_path)/MQTTPersistenceDefault.c \
	$(libpaho-mqtt3_lib_path)/Base64.c \
	$(libpaho-mqtt3_lib_path)/MQTTProperties.c \
	$(libpaho-mqtt3_lib_path)/MQTTReasonCodes.c \
	$(libpaho-mqtt3_lib_path)/MQTTTime.c \
	$(libpaho-mqtt3_lib_path)/MQTTVersion.c \
	$(libpaho-mqtt3_lib_path)/OsWrapper.c \
	$(libpaho-mqtt3_lib_path)/SHA1.c \
	$(libpaho-mqtt3_lib_path)/WebSocket.c \

libpaho-mqtt3_local_src_c_files_c := \
	$(libpaho-mqtt3_lib_path)/MQTTClient.c \

libpaho-mqtt3_local_src_c_files_cs := \
	$(libpaho-mqtt3_lib_path)/MQTTClient.c \
	$(libpaho-mqtt3_lib_path)/SSLSocket.c \

libpaho-mqtt3_local_src_c_files_a := \
	$(libpaho-mqtt3_lib_path)/MQTTAsync.c \
	$(libpaho-mqtt3_lib_path)/MQTTAsyncUtils.c \	

libpaho-mqtt3_local_src_c_files_as := \
	$(libpaho-mqtt3_lib_path)/MQTTAsync.c \
	$(libpaho-mqtt3_lib_path)/MQTTAsyncUtils.c \
	$(libpaho-mqtt3_lib_path)/SSLSocket.c \

libpaho-mqtt-helper_local_src_files := \
	$(libpaho-mqtt-helper_lib_path)/HelperUtils.c \
	$(libpaho-mqtt-helper_lib_path)/MQTTConnectHelper.c \
	$(libpaho-mqtt-helper_lib_path)/MQTTDisconnectHelper.c \
	$(libpaho-mqtt-helper_lib_path)/MQTTTopicSubscribeHelper.c \
	$(libpaho-mqtt-helper_lib_path)/MQTTTopicUnsubscribeHelper.c \

SED_COMMAND = sed \
    -e "s/@CLIENT_VERSION@/${VERSION}/g" \
    -e "s/@BUILD_TIMESTAMP@/${shell date}/g"

$(LOCAL_PATH)/$(build_path)/VersionInfo.h: $(LOCAL_PATH)/$(libpaho-mqtt3_lib_path)/VersionInfo.h.in
	$(SED_COMMAND) $< > $@

include $(CLEAR_VARS)
LOCAL_MODULE    := libssl
LOCAL_SRC_FILES := $(openssl_prebuild_dir)/$($(TARGET_ARCH_ABI)_openssl_prebuild_name)/lib/libssl.so
LOCAL_EXPORT_C_INCLUDES := $(openssl_prebuild_dir)/$($(TARGET_ARCH_ABI)_openssl_prebuild_name)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libcrypto
LOCAL_SRC_FILES := $(openssl_prebuild_dir)/$($(TARGET_ARCH_ABI)_openssl_prebuild_name)/lib/libcrypto.so
LOCAL_EXPORT_C_INCLUDES := $(openssl_prebuild_dir)/$($(TARGET_ARCH_ABI)_openssl_prebuild_name)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
.PHONY: $(LOCAL_PATH)/$(build_path)/VersionInfo.h
LOCAL_MODULE    := libpaho-mqtt-helper
LOCAL_SHARED_LIBRARIES := libdl libssl libcrypto
LOCAL_CFLAGS += -DOPENSSL
LOCAL_C_INCLUDES:= $(libpaho-mqtt3_c_includes) ../Prebuilt-OpenSSL-Android/Prebuilt/armv7-shared/include
LOCAL_SRC_FILES := $(libpaho-mqtt-helper_local_src_files) $(libpaho-mqtt3_local_src_c_files_common) $(libpaho-mqtt3_local_src_c_files_as)
include $(BUILD_SHARED_LIBRARY)
