LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := primitive-jni
LOCAL_SRC_FILES := primitive.c
LOCAL_LDLIBS    += -llog  
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := javastring-jni
LOCAL_SRC_FILES := javastring.c
LOCAL_LDLIBS    += -llog  
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := java-jni
LOCAL_SRC_FILES := java-jni.c
LOCAL_LDLIBS    += -llog  
include $(BUILD_SHARED_LIBRARY)
#/////begin toast_samplecode_03
include $(CLEAR_VARS)
LOCAL_MODULE    := jnitoast-jni
LOCAL_SRC_FILES := jnitoast.c
LOCAL_LDLIBS    += -llog
include $(BUILD_SHARED_LIBRARY)
#/////end
include $(CLEAR_VARS)
LOCAL_MODULE    := nio-jni
LOCAL_SRC_FILES := nio.c
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := checkjni-jni
LOCAL_SRC_FILES := checkjni.c
LOCAL_LDLIBS    := -llog	
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := addapp-jni
LOCAL_SRC_FILES := addapp.c
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := exception-jni
LOCAL_SRC_FILES := exception.c
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := jnireference-jni
LOCAL_SRC_FILES := jnireference.c
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)
