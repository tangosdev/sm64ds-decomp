typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern short data_02082214[];
extern void* data_ov007_0210342c;
extern int data_ov007_02103340;
extern int data_ov007_02103344;

extern void* func_ov007_020aebac(void);
extern void func_ov007_020bdeb0(int a);
extern int _ZN4cstd3modEii(int a, int b);
extern int _ZN4cstd3divEii(int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Matrix4x3_LoadIdentity(void* mat);
extern void func_ov007_020c39f8(void* mat, int rx, int ry, int rz);

struct V3 { int x, y, z; };

#pragma opt_common_subs off
void func_ov007_020b0da0(void* arg0) {
    char* self = (char*)arg0;
    char* obj = *(char**)(self + 4);
    char* st = *(char**)((char*)data_ov007_0210342c + 8);
    int counter = *(int*)(st + 0xc);
    char* mat = *(char**)(*(char**)(obj + 0) + 0xc);
    void* ret = func_ov007_020aebac();

    if (counter == 1) {
        *(int*)(obj + 0x94) = 1;
        *(int*)(st + 4) = 1;
        data_ov007_02103340 = 0;
        data_ov007_02103344 = 0;
        if (ret == 0) {
            *(int*)(*(char**)((char*)data_ov007_0210342c + 0x100) + 0x18) = 0;
        }
    }

    {
        struct V3 out;
        out.y = 0;
        out.x = 0;
        out.z = 0;

        if (!(*(volatile int*)(st + 4) & 2) && counter >= 4) {
            char* rp = *(char**)((char*)data_ov007_0210342c + 0x50);
            if (*(u16*)(rp + 0xc) != 0) {
                int a = *(u16*)(rp + 8);
                if (a >= 0x60 && a <= 0xa0) {
                    int b = *(u16*)(rp + 0xa);
                    if (b >= 0x40 && b <= 0x80) {
                        int* pflag = (int*)(((long long)(int)(st + 4)) & 0xFFFFFFFFFFFFFFFFLL);
                        *pflag |= 2;
                        data_ov007_02103340 = counter;
                        func_ov007_020bdeb0(0x34);
                        {
                            char* t0 = *(char**)((char*)data_ov007_0210342c + 0xc4);
                            char* t1 = *(char**)(t0 + 0);
                            char* t2 = *(char**)(t1 + 4);
                            *(short*)(t2 + 2) = 4;
                        }
                    }
                }
            }
        }

        if (*(int*)(st + 4) & 1) {
            int var_r4;
            _ZN4cstd3modEii(counter, 0x28);
            {
                int m1 = _ZN4cstd3modEii(counter, 0x28);
                if (m1 <= 0) {
                    var_r4 = 0;
                } else if (m1 >= 0x28) {
                    var_r4 = 0x1000;
                } else {
                    var_r4 = _ZN4cstd3divEii(m1 << 12, 0x28);
                }
            }

            {
                int angle1 = (u16)((var_r4 * 0xffff) >> 12);
                out.y = data_02082214[(angle1 >> 4) * 2] >> 2;
            }

            {
                int var_r8;
                int m2 = _ZN4cstd3modEii(counter, 0xa0);
                if (m2 <= 0) {
                    var_r8 = 0;
                } else if (m2 >= 0xa0) {
                    var_r8 = 0x1000;
                } else {
                    var_r8 = _ZN4cstd3divEii(m2 << 12, 0xa0);
                }

                Matrix4x3_LoadIdentity(mat);

                {
                    int angleA = (u16)(((var_r8 * 2 + 0x800) * 0xffff) >> 12);
                    int valA = data_02082214[(angleA >> 4) * 2];
                    int angleB = (u16)((var_r8 * 0xffff) >> 12);
                    int valB = data_02082214[(angleB >> 4) * 2];

                    int valBhalf = (valB + 0x2000) / 2;
                    int term = (int)(((s64)valA * 0x82 + 0x800) >> 12);
                    int combined = (int)(((s64)valBhalf * term + 0x800) >> 12);
                    short rzS = (short)(0x3fff + ((combined * 0x7fff) >> 12));
                    func_ov007_020c39f8(mat, 0, 0, (u16)rzS);
                }

                if ((*(int*)(st + 4) & 2) && (var_r4 == 0 || var_r4 == 0x800)) {
                    *(int*)(*(char**)((char*)data_ov007_0210342c + 0x100) + 0x18) = 0;
                }
            }
        }

        if (*(int*)(st + 4) & 4) {
            int diff = counter - data_ov007_02103344;
            int ratio;
            int sq;
            if (diff <= 0) {
                ratio = 0;
            } else if (diff >= 0x50) {
                ratio = 0x1000;
            } else {
                ratio = _ZN4cstd4fdivEii(diff, 0x50);
            }
            sq = (int)(((s64)ratio * ratio) >> 12);
            out.y = sq * 30;
            if (ratio >= 0x1000) {
                *(int*)(*(char**)((char*)data_ov007_0210342c + 0x100) + 0x18) = 0;
            }
        }

        *(struct V3*)(mat + 0x24) = out;
    }
}
