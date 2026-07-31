// @symbol func_ov006_020fa13c
// @emits dScMgMCarlo2_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgMCarlo2_c::Behavior - recovered from vtable slot identity */
typedef short s16;

extern int func_ov006_020c1718(void* p);
extern void func_ov006_020c19d0(void* c);


int dScMgMCarlo2_c_Behavior(void* arg)
{
    unsigned char* c = (unsigned char*)arg;

    switch (*(s16*)(c + 0x5928)) {
    case 1:
        {
            s16* p = (s16*)(((int)c + 0x5928));
            (*p)++;
        }
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            *(s16*)(c + 0xc0) = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d700 = *(s16*)(c + 0x592e) * 5 << 12;
        if (*(s16*)(c + 0x592e) == 4) {
            if (func_ov006_020f9668() == 0) {
                *(s16*)(c + 0x592e) = 0;
                {
                    s16* p = (s16*)(((int)c + 0x5928));
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
                *(s16*)(c + 0x592e) = 0;
                *(s16*)(c + 0x511e) = 0;
                func_ov006_020c1164(c + 0x4f38, 2, c + 0x592e);
                if (data_ov006_0213d6f4 == 2)
                    *(s16*)(c + 0x4f52) = 0;
            } else {
                int lim = *(s16*)(c + 0x592e) + 0x12;
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
                        if (*(s16*)(c + 0x4f52) == 1) {
                            *(s16*)(c + 0x4f52) = 0;
                        } else if (func_ov006_020c16b4(c + 0x4f38) != 0) {
                            func_ov006_020c0d68(c + 0x4f38);
                            func_ov004_020b0a54(0x12);
                            c[0xc3] = 0;
                            {
                                s16* p = (s16*)(((int)c + 0x5928));
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
