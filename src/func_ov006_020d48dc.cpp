//cpp
#pragma opt_strength_reduction off
#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;

struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual int m_90();   /* slot 36 -> 0x90 */
};

typedef struct { int v[14]; } Buf14;

extern "C" void SetSubBg2Offset(int a, int b);
extern "C" void func_ov004_020b1e34(void* a0, int a1, int a2, int a3);
extern "C" void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov006_020d47f4(void* c);
extern "C" void func_ov006_020d452c(void* c);

extern short data_02082214[];
extern Buf14 data_ov006_0213b880;
extern void* data_ov006_0213a458[];
extern void* data_ov006_0213a4b0[];
extern u8 data_0209d45c;
extern u8 data_0209d454;

extern "C" int func_ov006_020d48dc(char* self)
{
    *(u16*)(((long long)(int)(self + 0x53bc))) += 0xc0;
    {
        u8 *p2 = (u8*)(self + 0x5300);
        u16 idx = *(u16*)(p2 + 0xbc);
        int v = data_02082214[(idx >> 4) << 1];
        int off = (v + (int)((unsigned)(v >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, 0);
    }
    func_ov004_020b1e34(self, 0xe0, 0x14, 1);

    if (*(int*)(self + 0x46d0) == 3 && *(int*)(self + 0x53d0) == 0) {
        data_0209d45c &= ~1;
        data_0209d454 &= ~1;
        if (((Base*)self)->m_90() == 0 && *(u8*)(self + 0x46d4) == 1) {
            int i;
            for (i = 0; i < 4; i++) {
                if (*(int*)(self + i * 4 + 0x4714) != 0) {
                    int *counterA = (int*)(((long long)(int)(self + i * 4 + 0x539c)));
                    (*counterA)++;
                    Buf14 local = data_ov006_0213b880;
                    if (*counterA >= local.v[i]) {
                        int *counterB;
                        *counterA = 0;
                        counterB = (int*)(((long long)(int)(self + i * 4 + 0x53ac)));
                        (*counterB)++;
                        if (*counterB >= 0xe)
                            *counterB = 0;
                    }
                    func_ov004_020afdd0(
                        data_ov006_0213a458[*(int*)(self + i * 4 + 0x53ac)],
                        *(int*)(self + i * 8 + 0x4724) >> 12,
                        (*(int*)(self + i * 8 + 0x4728) >> 12) - 4,
                        -1,
                        0);
                }
            }

            int r8_val = 0;
            int r7_val = 0;
            int r4_val = -1;
            for (; r8_val < 0x80; r8_val++, self += 0x18) {
                if (*(u8*)(self + 0x477c) != 0) {
                    int ang = *(int*)(self + 0x4778);
                    int sb_val = *(int*)(self + 0x4768);
                    int idx = ang / 4;
                    int r2_val = *(int*)(self + 0x476c);
                    func_ov004_020afdd0(data_ov006_0213a4b0[idx], sb_val >> 12, r2_val >> 12, r4_val, r7_val);
                }
            }

        }
        return 1;
    } else {
        data_0209d45c |= 1;
        data_0209d454 |= 1;
        if (((Base*)self)->m_90() != 0) {
            func_ov006_020d47f4(self);
        } else {
            func_ov006_020d452c(self);
        }
        return 1;
    }
}
