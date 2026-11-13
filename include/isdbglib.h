#ifndef ISDBGLIB_H
#define ISDBGLIB_H
#ifndef __cplusplus
#define _ISDbgLib_UnregistOverlayInfo _Z29_ISDbgLib_UnregistOverlayInfo11OVERLAYPROCjj
#define _ISDbgLib_RegistOverlayInfo _Z27_ISDbgLib_RegistOverlayInfo11OVERLAYPROCjjj
#endif

/* The IS-NITRO debugger library's overlay registration, as NitroSDK's
 * fs_overlay.c calls it from <isdbglib.h> (outside SDK_FINALROM builds).
 * isdoverlay.cpp defines both; this ROM's FS_EndOverlay and the game's
 * FS_LoadOverlayImage copy (func_02018b64) call them. Only the names
 * fs_overlay.c uses are declared here. */

#include "types.h"

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif

/* Which processor's overlay table an entry goes in. */
typedef enum { OVERLAYPROC_ARM9, OVERLAYPROC_ARM7 } OVERLAYPROC;

int _ISDbgLib_RegistOverlayInfo(OVERLAYPROC, u32, u32, u32);
int _ISDbgLib_UnregistOverlayInfo(OVERLAYPROC, u32, u32);

#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif /* ISDBGLIB_H */
