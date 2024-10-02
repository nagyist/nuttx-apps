MY_COMMON_PATH := vendor/vela/apps/netutils

PRODUCT_PACKAGES += rpmsg_tun_client

PRODUCT_PRIVATE_SEPOLICY_DIRS += \
    $(MY_COMMON_PATH)/rpmsg_tun/sepolicy

