#!/system/bin/sh
#
# This script is Needed to replace the FP hal for egis devices
# Fix by @sarthakroy2002
#

mount -o rw /vendor

FP=$(cat /proc/fp_id)

if [ $FP = "E_520" ]; then
    echo "EGIS FP Detected"
    cp -r /vendor/fptmp/android.hardware.biometrics.fingerprint@2.1-service.r5x /vendor/bin/hw/
    cp -r /vendor/fptmp/libets_teeclient_v2.so /vendor/lib64/hw/
fi 

rm -rf /vendor/fptmp
