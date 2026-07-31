// @symbol func_02034d70
// @emits dScMB_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Scene.h"
/* recovered: renamed to Class_Method */
/* dScMB_c::CleanupResources - recovered from vtable slot identity */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;


extern void* SCENE_RELATED;  /* 0x0209d4a8 */

int dScMB_c_CleanupResources(void) {
    SCENE_RELATED = 0;
    _ZN5Scene20SetAndStopColorFaderEv();
    return 1;
}
