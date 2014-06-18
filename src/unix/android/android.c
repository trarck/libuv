#if defined(ANDROID_PLATFORM) && ANDROID_PLATFORM<9
#include "pthread-rwlock.c"
#include "fdatasync.c"
#include "sysinfo.c"
#include "stime.c"
#endif