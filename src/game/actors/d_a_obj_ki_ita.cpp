//cpp
/**
 * Jolly Roger Bay floating plank (`ita` = board).
 *
 * No fields. InitResources hands this overlay's model and collision
 * files to daObjFloatBoard_c's shared ov002 helper. CleanupResources
 * is the base's.
 *
 * daObjKi_Ita_c_classInit / g_profile_KI_ITA are reconstructed (RTTI
 * daObjKi_Ita_c, KI_ITA registry). Retail does not store those spellings.
 *
 * data_ov016_02114b8c is the model, collision and CLPS row between
 * this class's RTTI and type-name string.
 */

#include "daObjKi_Ita_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

extern "C" daObjFloatBoard_c_Resources data_ov016_02114b8c;

/* File 1601 / 1602 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * ModelFile::ModelFile / ModelFile::~ModelFile and ClsnFile::ClsnFile /
 * ClsnFile::~ClsnFile. */
struct KiItaModelFilePtr : SharedFilePtr {
    KiItaModelFilePtr(u32 fileID);
    ~KiItaModelFilePtr();
};

struct KiItaCollisionFilePtr : SharedFilePtr {
    KiItaCollisionFilePtr(u32 fileID);
    ~KiItaCollisionFilePtr();
};

daObjKi_Ita_c *daObjKi_Ita_c_classInit() {
    return new daObjKi_Ita_c();
}

extern "C" SpawnInfo<daObjKi_Ita_c> g_profile_KI_ITA = {
    daObjKi_Ita_c_classInit,
    0x003c, /* KI_ITA registry id 60 */
    0x00b8,
    2,
    0,
    0x00250000,
    0x02000000,
    0,
};

int daObjKi_Ita_c::InitResources() {
    return InitResourcesFrom(&data_ov016_02114b8c);
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the ROM-ascending run. Construction order is the model
 * handle, then the collision handle. */
KiItaModelFilePtr data_ov016_02114e44(1601);
KiItaCollisionFilePtr data_ov016_02114e4c(1602);

extern struct CLPS_Block data_ov016_02113bec;
daObjFloatBoard_c_Resources data_ov016_02114b8c = {
    &data_ov016_02114e44,
    &data_ov016_02114e4c,
    &data_ov016_02113bec,
};

/* CLPS property block for the floating plank: the 'CLPS' tag, the 8-byte
 * stride header with one entry, then the default entries. */
