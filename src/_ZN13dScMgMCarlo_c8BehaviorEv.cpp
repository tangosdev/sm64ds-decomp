//cpp
// @symbol _ZN13dScMgMCarlo_c8BehaviorEv
#include "decl_common.h"
#include "dScMgMCarlo_c.h"
/* dScMgMCarlo_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: the one slot where this class's table differs
 * from dScMgSingle3DBase_c's. The old file's `recovered name:` comment agreed, and
 * here it is right -- unlike the four the reference commit had to move up a level.
 *
 * The round's state machine, on the s16 at 0x60a8. Case 1 arms the shared
 * dScMgBase_c counter pair at 0xc3/0xc4/0xc0 -- the identical idiom Flower's and
 * Snowball's own Behaviors use, which is why include/dScMgBase_c.h names those
 * three fields. It then falls through into case 2 deliberately.
 *
 * WAS A C99 FILE. Two consequences, both mechanical:
 *   - the declarations move inside `extern "C"`, or they would mangle;
 *   - `data_ov004_020beb68` is NOT redeclared here. include/dScMgBase_c.h already
 *     declares it `void *`, and the old file's local `extern char *` would now be
 *     a conflicting declaration rather than a private convenience. It is cast at
 *     each use instead, which loads the same word.
 * The local `typedef short s16;` is gone for the same reason: types.h arrives
 * through decl_common.h and already has it. */

extern "C" {
int func_ov006_020c1718(void* p);
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(void* c);
extern int data_ov006_0213d568;
}

s32 dScMgMCarlo_c::Behavior()
{
    void *arg = (void *)this;
    unsigned char* c = (unsigned char*)arg;

    switch (*(s16*)(c + 0x60a8)) {
    case 1:
        {
            s16* p = (s16*)(((int)c + 0x60a8));
            (*p)++;
        }
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            *(s16*)(c + 0xc0) = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d574 = *(s16*)(c + 0x60ae) * 5 << 12;
        if (*(s16*)(c + 0x60ae) == 4) {
            if (func_ov006_020f7b10() == 0) {
                *(s16*)(c + 0x60ae) = 0;
                {
                    s16* p = (s16*)(((int)c + 0x60a8));
                    (*p)++;
                }
            }
        }
        break;
    case 3:
        if (func_ov006_020f7a90() == 0) {
            int r5;
            if (data_ov006_0213d564 == 1)
                data_ov006_0213d564 = 0;
            r5 = data_ov006_0213d574 >> 12;
            if (data_ov006_0213d56c != 0 && r5 > 10 && r5 <= 18
                && func_ov006_020c1718(c + 0x4f38) != 0) {
                *(s16*)(c + 0x60ae) = 0;
                *(s16*)(c + 0x511e) = 0;
                func_ov006_020c1164(c + 0x4f38, 2, c + 0x60ae);
                if (data_ov006_0213d56c == 2)
                    *(s16*)(c + 0x4f52) = 0;
            } else {
                int lim = *(s16*)(c + 0x60ae) + 0x12;
                if (r5 >= lim) {
                    int flag = (data_ov006_021424fc != 0 && data_ov006_02142508 != 0);
                    if (flag == 0)
                        data_ov006_0213d574 = lim << 12;
                }
            }
        } else {
            if (data_ov006_0213d564 == 0) {
                if (func_ov006_020f7b90() != 0) {
                    data_ov006_0213d564 = 1;
                } else {
                    if (data_ov006_0213d570 != 0) {
                        if (*(s16*)(c + 0x4f52) == 1) {
                            *(s16*)(c + 0x4f52) = 0;
                        } else if (func_ov006_020c16b4(c + 0x4f38) != 0) {
                            {
                                char *g = (char *)data_ov004_020beb68;
                                if (g != 0) {
                                    if (*(int *)(g + 0xb4) > 0)
                                        *(int *)(((int)g + 0xb4)) -= 1;
                                }
                            }
                            func_ov006_020c0d68(c + 0x4f38);
                            func_ov004_020b0a54(5);
                            {
                                int d = data_ov006_0213d570;
                                char *g = (char *)data_ov004_020beb68;
                                int t = ((g != 0) ? *(int *)(g + 0xa8) : 0) - d;
                                int u = (g != 0) ? *(int *)(g + 0xb4) : 0;
                                func_ov004_020ad79c(t, u);
                            }
                            c[0xc3] = 0;
                            {
                                s16* p = (s16*)(((int)c + 0x60a8));
                                (*p)++;
                            }
                        }
                    } else {
                        {
                            char *g = (char *)data_ov004_020beb68;
                            if (g != 0) {
                                if (*(int *)(g + 0xb4) < 0x270f)
                                    *(int *)(((int)g + 0xb4)) += 1;
                                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
                            }
                        }
                        func_ov004_020adb1c(*(int *)(c + 0xb4));
                        func_ov006_020c0c80(c + 0x4f38);
                        func_ov004_020b0a54(4);
                        {
                            char *g = (char *)data_ov004_020beb68;
                            int t = (g != 0) ? *(int *)(g + 0xa8) : 0;
                            func_ov004_020ad79c(data_ov006_0213d568 + t,
                                                (g != 0) ? *(int *)(g + 0xb4) : 0);
                        }
                        c[0xc3] = 0;
                        {
                            s16* p = (s16*)(((int)c + 0x60a8));
                            (*p)++;
                        }
                    }
                    data_ov006_0213d564 = 0;
                }
            }
        }
        break;
    }

    func_ov004_020b65e4();
    func_ov006_020c19d0(c + 0x4f38);
    func_ov006_020f7740();
    return 1;
}
