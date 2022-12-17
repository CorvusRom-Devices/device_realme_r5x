/*
   Copyright (c) 2014, The Linux Foundation. All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <sys/sysinfo.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include <android-base/properties.h>
#include "vendor_init.h"

#include <fs_mgr_dm_linear.h>

using android::base::GetProperty;
using android::base::ReadFileToString;

char const *heapminfree;
char const *heapmaxfree;
char const *heaptargetutilization;

struct r5x_props
{
    std::string build_fingerprint;
    std::string device_build;
    std::string product_device;
};

std::vector<std::string> ro_props_default_source_order = {
    "",
    "odm.",
    "product.",
    "system.",
    "vendor.",
    "system_ext.",
};

void init_dalvik()
{
    struct sysinfo sys;

    sysinfo(&sys);

    if (sys.totalram >= 3ull * 1024 * 1024 * 1024){
        // from - phone-xhdpi-4096-dalvik-heap.mk
        heaptargetutilization = "0.6";
        heapminfree = "8m";
        heapmaxfree = "16m";
    } else if (sys.totalram >= 2ull * 1024 * 1024 * 1024) {
        // from - phone-xhdpi-2048-dalvik-heap.mk.mk
        heaptargetutilization = "0.75";
        heapminfree = "512k";
        heapmaxfree = "8m";
    }
}

void property_override(char const prop[], char const value[], bool add = true)
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void init_fp_properties()
{
    char const *fp_name_file = "/proc/fp_id";
    std::string fp_name;

    if (ReadFileToString(fp_name_file, &fp_name)) {
        if (fp_name == "E_520") {
            property_override("persist.vendor.fingerprint.fp_id", "E_520");
        }
    }
}

void setRMX(unsigned int variant)
{
    r5x_props prop[4] = {};

    //RMX1911
    prop[0] = {
        "realme/RMX1911/RMX1911:10/QKQ1.200209.002/1608537052:user/release-keys",
        "RMX1911",
        "RMX1911",
    };

    // RMX1925
    prop[1] = {
        "realme/RMX1925/RMX1925:10/QKQ1.200209.002/1608537052:user/release-keys",
        "RMX1925",
        "RMX1925",
    };

    //RMX1927
    prop[2] = {
        "realme/RMX1927/RMX1927:10/QKQ1.200209.002/1608537052:user/release-keys",
        "RMX1927",
        "RMX1927",
    };

    //RMX2030
    prop[3] = {
        "realme/RMX2030/RMX2030:10/QKQ1.200209.002/1608537052:user/release-keys",
        "RMX2030",
        "RMX2030",
    };

    const auto set_ro_build_prop = [](const std::string &source,
                                      const std::string &prop, const std::string &value) {
        auto prop_name = "ro." + source + "build." + prop;
        property_override(prop_name.c_str(), value.c_str());
    };

    const auto set_ro_product_prop = [](const std::string &source,
                                        const std::string &prop, const std::string &value) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str());
    };

    property_override("ro.build.product", prop[variant].product_device.c_str());
    for (const auto &source : ro_props_default_source_order)
    {
        set_ro_build_prop(source, "fingerprint", prop[variant].build_fingerprint.c_str());
        set_ro_product_prop(source, "device", prop[variant].product_device.c_str());
        set_ro_product_prop(source, "model", prop[variant].device_build.c_str());
    }
    property_override("ro.bootimage.build.fingerprint", prop[variant].build_fingerprint.c_str());
}

void init_device_model()
{
    std::ifstream infile("/proc/oppoVersion/prjVersion");
    std::string prjVersion;
    getline(infile, prjVersion);

    std::ifstream fin;
    std::string buf;
    fin.open("/proc/cmdline");
    while (std::getline(fin, buf, ' '))
      if (buf.find("board_id") != std::string::npos)
          break;
    fin.close();

    if (prjVersion == "19743")
    {
        setRMX(3); //RMX2030
    }
    else if (prjVersion == "19632")
    {
        setRMX(2); //RMX1927
    }
    else
    {
        if (buf.find("S86125AA1") != std::string::npos) {
            setRMX(1); // RMX1925
        } else {
            setRMX(0); //RMX1911
        }
    }
}

void vendor_load_properties() {
    init_device_model();
    init_fp_properties();

    init_dalvik();
    property_override("dalvik.vm.heapstartsize", "8m", true);
    property_override("dalvik.vm.heapgrowthlimit", "192m", true);
    property_override("dalvik.vm.heapsize", "512m", true);
    property_override("dalvik.vm.heaptargetutilization", heaptargetutilization, true);
    property_override("dalvik.vm.heapminfree", heapminfree, true);
    property_override("dalvik.vm.heapmaxfree", heapmaxfree, true);

#ifdef __ANDROID_RECOVERY__
    std::string buildtype = GetProperty("ro.build.type", "userdebug");
    if (buildtype != "user") {
        property_override("ro.debuggable", "1");
        property_override("ro.adb.secure.recovery", "0");
    }
#endif
}
