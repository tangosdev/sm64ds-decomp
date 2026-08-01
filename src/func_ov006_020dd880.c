//cpp
#include "types.h"
extern "C" int RandomIntInternal(int *seed);
extern "C" void func_ov006_020ddcf8(char *c, int idx);

extern int data_ov006_0212e3d0[];
extern int data_ov006_0212e3e8[];
extern int data_0209d4b8;

#define LB(a) (*(unsigned char*)(((long long)(int)(a))))

struct Obj {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19();
    virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23();
    virtual int v24(); virtual int v25(); virtual int v26(); virtual int v27();
    virtual int v28(); virtual int v29(); virtual int v30(); virtual int v31();
    virtual int v32(); virtual int v33(); virtual int v34(); virtual int v35();
};

extern "C" void func_ov006_020dd880(char *c, int i)
{
    int n = i * 0x1c;
    int idxb;
    int diff;
    int base_j;
    int fp;
    int j;
    int *pD;

    *(int *)(c + 0x4660 + n) += *(int *)(c + 0x4668 + n);
    *(int *)(c + 0x4664 + n) += *(int *)(c + 0x466c + n);
    *(int *)(c + 0x466c + n) += 0x400;
    func_ov006_020ddcf8(c, i);

    pD = (int *)(c + 0x466c + n);
    idxb = *(u8 *)(c + n + 0x4674);
    diff = data_ov006_0212e3d0[idxb] - data_ov006_0212e3e8[idxb];
    if ((*(int *)(c + 0x4664 + n) >> 12) > diff) {
        *(int *)(c + 0x4664 + n) = diff * 0x1000;
        *pD = 0;
        if (*(int *)(c + 0x4668 + n) == 0) {
            unsigned int rv = (unsigned int)RandomIntInternal(&data_0209d4b8);
            if (((((rv >> 16) & 0x7fff) << 1) >> 15))
                *(int *)(c + 0x4668 + n) = 0xc00;
            else
                *(int *)(c + 0x4668 + n) = -0xc00;
        }
    }
    {
        int bval = *(int *)(c + 0x4664 + n);
        if (bval != (diff << 12))
            return;
    }

    base_j = (idxb - 3) * 8;
    fp = *(int *)(c + 0x4660 + n) >> 12;
    for (j = 0; j < 8; j++) {
        int idx = base_j + j;
        int d = (*(int *)((c + idx * 0x18) + 0x4ac0) >> 12) - fp;
        int limit = 2;
        char *e = c + idx * 0x18;
        int res = ((Obj *)c)->v35();
        if (res != 0) limit = 5;
        if (d <= -6 || d >= 6) continue;
        if (*(u8 *)(e + 0x4ad4) != 0) continue;
        if (LB(e + 0x4ad3) > limit) continue;
        LB(e + 0x4ad3) += 1;
        *(u8 *)(c + n + 0x4675) = 3;
        *pD = 0x200;
        *(u8 *)(c + n + 0x467b) = (u8)idx;
        return;
    }
}
