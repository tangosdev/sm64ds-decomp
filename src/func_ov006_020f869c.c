// @symbol func_ov006_020f869c
// @emits dScMgMCarlo_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgMCarlo_c::Behavior - recovered from vtable slot identity */
typedef short s16;

extern int func_ov006_020c1718(void* p);
extern void func_ov004_020b65e4(void);
extern void func_ov006_020c19d0(void* c);

extern char *func_020beb68;
extern int data_ov006_0213d568;

int dScMgMCarlo_c_Behavior(void* arg)
{
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
                                char *g = func_020beb68;
                                if (g != 0) {
                                    if (*(int *)(g + 0xb4) > 0)
                                        *(int *)(((int)g + 0xb4)) -= 1;
                                }
                            }
                            func_ov006_020c0d68(c + 0x4f38);
                            func_ov004_020b0a54(5);
                            {
                                int d = data_ov006_0213d570;
                                char *g = func_020beb68;
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
                            char *g = func_020beb68;
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
                            char *g = func_020beb68;
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
