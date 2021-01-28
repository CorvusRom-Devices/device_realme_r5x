package trinket

import (
    "android/soong/android"
)

func init() {
    android.RegisterModuleType("realme_trinket_init_library_static", initLibraryFactory)
}
