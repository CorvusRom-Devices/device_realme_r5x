#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Kernel modules
KERNEL_MODULES_ORIG := $(LOCAL_PATH)/modules
ifeq ($(NEED_KERNEL_MODULE_VENDOR_OVERLAY),true)
KERNEL_MODULES_DEST := $(TARGET_COPY_OUT_PRODUCT)/vendor_overlay/30/lib/modules
else
KERNEL_MODULES_DEST := $(TARGET_COPY_OUT_VENDOR)/lib/modules
endif

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(KERNEL_MODULES_ORIG)/,$(KERNEL_MODULES_DEST))
