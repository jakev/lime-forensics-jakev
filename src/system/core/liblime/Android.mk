#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)


liblime_sources := lime_core.c

# Static library for host
# ========================================================
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := liblime
LOCAL_SRC_FILES := $(liblime_sources)
include $(BUILD_HOST_STATIC_LIBRARY)

# Shared and static library for target
# ========================================================
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := liblime
LOCAL_SRC_FILES := $(liblime_sources)
LOCAL_SHARED_LIBRARIES := liblog
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := liblime
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_WHOLE_STATIC_LIBRARIES := liblime
include $(BUILD_SHARED_LIBRARY)
