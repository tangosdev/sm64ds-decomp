#ifndef EXTENDINGPLATFORM_H
#define EXTENDINGPLATFORM_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbgSclY.h"

struct ExtendingPlatform {
    u8  pad_000[0x8e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    s8  mGrowing;            /* 0x0d4 */
    u8  pad_0d5[0x3];
    Model mModel;            /* 0x0d8 */
    u8  pad_128[0x30];
    dBgW_KcMbgSclY mCollider; /* 0x158 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char ExtendingPlatform_size_must_be_0x328[
    sizeof(struct ExtendingPlatform) == 0x328 ? 1 : -1];

#endif
