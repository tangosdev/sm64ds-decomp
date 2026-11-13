#ifndef NITRO_FX_FX_CP_H
#define NITRO_FX_FX_CP_H
#ifndef __cplusplus
#define FX_ModS32 _Z9FX_ModS32ii
#define FX_DivS32 _Z9FX_DivS32ii
#define FX_DivAsync _Z11FX_DivAsyncii
#define FX_GetSqrtResult _Z16FX_GetSqrtResultv
#define FX_SqrtAsync _Z12FX_SqrtAsynci
#define FX_InvAsync _Z11FX_InvAsynci
#define FX_GetDivResult _Z15FX_GetDivResultv
#define FX_GetDivResultFx64c _Z20FX_GetDivResultFx64cv
#define FX_InvSqrt _Z10FX_InvSqrti
#define FX_Sqrt _Z7FX_Sqrti
#define FX_Inv _Z6FX_Invi
#define FX_DivFx64c _Z11FX_DivFx64cii
#define FX_Div _Z6FX_Divii
#endif

/* NitroSDK's coprocessor divide and square-root API (nitro/fx/fx_cp.h),
 * defined in src/cstd.cpp, the ROM's FX_cp.c run (pret pokediamond
 * arm9/lib/NitroSDK/src/FX_cp.c). FX_DivFx64c, FX_InvSqrt and FX_SqrtAsync
 * are the later SDK's additions to the same file.
 *
 * Parameters stay unnamed: a named parameter in a declaration takes a
 * compiler @NNN number in every includer. */

#include "nitro/fx/fx.h"


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


#endif /* NITRO_FX_FX_CP_H */
