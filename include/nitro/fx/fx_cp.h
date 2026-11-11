#ifndef NITRO_FX_FX_CP_H
#define NITRO_FX_FX_CP_H

/* NitroSDK's coprocessor divide and square-root API (nitro/fx/fx_cp.h),
 * defined in src/cstd.cpp, the ROM's FX_cp.c run (pret pokediamond
 * arm9/lib/NitroSDK/src/FX_cp.c). FX_DivFx64c, FX_InvSqrt and FX_SqrtAsync
 * are the later SDK's additions to the same file.
 *
 * Parameters stay unnamed: a named parameter in a declaration takes a
 * compiler @NNN number in every includer. */

#include "nitro/fx/fx.h"

#ifdef __cplusplus
extern "C" {
#endif

fx32 FX_Div(fx32, fx32);
fx32 FX_Inv(fx32);
fx32 FX_Sqrt(fx32);
fx32 FX_InvSqrt(fx32);
fx64c FX_DivFx64c(fx32, fx32);
fx64c FX_GetDivResultFx64c(void);
fx32 FX_GetDivResult(void);
void FX_InvAsync(fx32);
void FX_DivAsync(fx32, fx32);
fx32 FX_GetSqrtResult(void);
void FX_SqrtAsync(fx32);
int FX_DivS32(int, int); /* s32 in the SDK */
int FX_ModS32(int, int); /* s32 in the SDK */

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FX_FX_CP_H */
