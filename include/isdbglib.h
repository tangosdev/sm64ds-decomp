#ifndef ISDBGLIB_H
#define ISDBGLIB_H

/* The IS-NITRO debugger library's overlay registration, as NitroSDK's
 * fs_overlay.c calls it from <isdbglib.h> (outside SDK_FINALROM builds).
 * isdoverlay.cpp defines both; this ROM's FS_EndOverlay and the game's
 * FS_LoadOverlayImage copy (func_02018b64) call them. Only the names
 * fs_overlay.c uses are declared here. */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Which processor's overlay table an entry goes in. */
typedef enum { OVERLAYPROC_ARM9, OVERLAYPROC_ARM7 } OVERLAYPROC;

int _ISDbgLib_RegistOverlayInfo(OVERLAYPROC, u32, u32, u32);
int _ISDbgLib_UnregistOverlayInfo(OVERLAYPROC, u32, u32);

#ifdef __cplusplus
}
#endif

#endif /* ISDBGLIB_H */
