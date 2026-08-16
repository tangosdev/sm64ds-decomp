//cpp
// @symbol _ZN14dScMgMCarlo2_c8BehaviorEv
#include "decl_common.h"
#include "dScMgMCarlo2_c.h"
/* dScMgMCarlo2_c::Behavior -- vtable slot 6, ov006 0x020fa13c.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * THE STATE COUNTER IS BUMPED THROUGH A LAUNDERED POINTER, three times, and
 * that is not a leftover: `unk_5928++` recomputes the address for the store
 * where `s16 *p = (s16 *)((int)this + 0x5928); (*p)++;` makes mwccarm CSE it
 * into a register, which is what the cartridge does. Same lever
 * notes/mwccarm-codegen.md records for compound assignment. Everything the
 * class owns outright and reads once -- unk_592e, unk_4f52, unk_511e -- reads
 * as a member.
 *
 * The 0x270-byte shared table at 0x4f38 stays opaque, as
 * include/dScMgMCarlo2_c.h's banner says: five siblings use the same one, and
 * the helpers here take its base address rather than anything inside it. The
 * two fields that ARE named within it, unk_4f52 and unk_511e, are named
 * because this function and its siblings write them by hand. */

extern "C" {
int func_ov006_020c1718(void *p);
void func_ov006_020c19d0(void *c);
}

s32 dScMgMCarlo2_c::Behavior()
{
    unsigned char *c = (unsigned char *)this;

    switch (unk_5928) {
    case 1:
        {
            s16 *p = (s16 *)((int)c + 0x5928);
            (*p)++;
        }
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            *(s16 *)(c + 0xc0) = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d700 = unk_592e * 5 << 12;
        if (unk_592e == 4) {
            if (func_ov006_020f9668() == 0) {
                unk_592e = 0;
                {
                    s16 *p = (s16 *)((int)c + 0x5928);
                    (*p)++;
                }
            }
        }
        break;
    case 3:
        if (func_ov006_020f95f0() == 0) {
            int r5;
            if (data_ov006_0213d6fc == 1)
                data_ov006_0213d6fc = 0;
            r5 = data_ov006_0213d700 >> 12;
            if (data_ov006_0213d6f4 != 0 && r5 > 10 && r5 <= 18
                && func_ov006_020c1718(c + 0x4f38) != 0) {
                unk_592e = 0;
                unk_511e = 0;
                func_ov006_020c1164(c + 0x4f38, 2, &unk_592e);
                if (data_ov006_0213d6f4 == 2)
                    unk_4f52 = 0;
            } else {
                int lim = unk_592e + 0x12;
                if (r5 >= lim) {
                    int flag = (data_ov006_02142570 != 0 && data_ov006_02142574 != 0);
                    if (flag == 0)
                        data_ov006_0213d700 = lim << 12;
                }
            }
        } else {
            if (data_ov006_0213d6fc == 0) {
                if (func_ov006_020f96e0() != 0) {
                    data_ov006_0213d6fc = 1;
                } else {
                    if (data_ov006_0213d6f8 != 0) {
                        if (unk_4f52 == 1) {
                            unk_4f52 = 0;
                        } else if (func_ov006_020c16b4(c + 0x4f38) != 0) {
                            func_ov006_020c0d68(c + 0x4f38);
                            func_ov004_020b0a54(0x12);
                            c[0xc3] = 0;
                            {
                                s16 *p = (s16 *)((int)c + 0x5928);
                                (*p)++;
                            }
                        }
                    }
                    data_ov006_0213d6fc = 0;
                }
            }
        }
        break;
    }

    func_ov006_020c19d0(c + 0x4f38);
    func_ov006_020f9000();
    return 1;
}
