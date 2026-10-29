//cpp
/* unit020b3edc -- ov007 unit 0x020b3edc..0x020b413c: 4 functions, .text only.
 *
 * The title menu's file cache. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. The module keeps 0x2e file slots,
 * data_ov007_02103370, and the pair of callbacks that load and release a
 * file, data_ov007_02103368 and data_ov007_0210336c. func_ov007_020b4100
 * empties the slots and installs the callbacks; func_ov007_020b3f20 returns
 * slot idx, loading it on first use through the load callback with the
 * file id from data_ov007_020d7584[idx]. On a load it prepares the file:
 * slots 0x26..0x2a and 0x2c are LZ16-compressed and are decompressed into
 * a new buffer (the compressed file is released), and the model,
 * animation and texture-pattern slots have their file offsets resolved.
 * func_ov007_020b3edc releases one slot and func_ov007_020b40c0 releases
 * them all and clears the callbacks.
 *
 * Edges: the bss words 0x02103368..0x02103428 are referenced by these
 * four functions and nothing else in the overlay. The previous function,
 * func_ov007_020b3e84, belongs to the module of bss 0x02103360
 * (unit020b2cdc); the next one, func_ov007_020b413c, touches none of this
 * module's data.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "CP15.h"
#include "dExtSimpleModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "dExtAnmTexPat_c.h"

extern "C" {

typedef void *(*Ov007FileLoad)(u16 id);
typedef void (*Ov007FileRelease)(void *file);

extern Ov007FileLoad data_ov007_02103368;
extern Ov007FileRelease data_ov007_0210336c;
extern void *data_ov007_02103370[];
/* the file id each slot loads */
extern u16 data_ov007_020d7584[];

extern int func_ov007_020c3df4(int a, void *b);
extern void DecompressLZ16(void *src, void *dst);

void func_ov007_020b3edc(int idx);

// @symbol func_ov007_020b4100
void func_ov007_020b4100(int load, int release) {
    int i;
    for (i = 0; i < 0x2e; i++) {
        data_ov007_02103370[i] = 0;
    }
    data_ov007_02103368 = (Ov007FileLoad)load;
    data_ov007_0210336c = (Ov007FileRelease)release;
}

// @symbol func_ov007_020b40c0
void func_ov007_020b40c0(void) {
    int i;
    for (i = 0; i < 0x2e; i++)
        func_ov007_020b3edc(i);
    data_ov007_02103368 = 0;
    data_ov007_0210336c = 0;
}

// @symbol func_ov007_020b3f20
void *func_ov007_020b3f20(int idx) {
    void *new_var;
    if (idx == 0) {
        return 0;
    }
    if (data_ov007_02103370[idx] == 0) {
        void *obj = data_ov007_02103368(data_ov007_020d7584[idx]);
        data_ov007_02103370[idx] = obj;
        obj = (new_var = data_ov007_02103370[idx]);
        switch (idx) {
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2c: {
            /* LZ16 header: the decompressed size in the top 24 bits */
            u32 hv = (*((u32 *)obj)) >> 8;
            int sz = func_ov007_020c3df4(0, (void *)hv);
            DecompressLZ16(obj, (void *)sz);
            data_ov007_0210336c(obj);
            CP15::FlushDataCache(sz, hv);
            data_ov007_02103370[idx] = (void *)sz;
        } break;

        case 1:
        case 2:
        case 3:
        case 0x16:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
            dExtSimpleModel_c::UpdateFileOffsets(*(BMD_File *)obj);
            break;

        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
            dExtFrameCtrl_c::UpdateFileOffsets(*(BCA_File *)obj);
            break;

        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
            dExtAnmTexPat_c::UpdateFileOffsets(*(BTP_File *)obj);
            break;

        default:
            break;
        }
    }
    return data_ov007_02103370[idx];
}

// @symbol func_ov007_020b3edc
void func_ov007_020b3edc(int idx) {
    void *file = data_ov007_02103370[idx];
    if (file == 0)
        return;
    data_ov007_0210336c(file);
    data_ov007_02103370[idx] = 0;
}
}
