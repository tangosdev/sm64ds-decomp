/* func_ov006_021104c0 at 0x021104c0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int _ZN4cstd4fdivEii(int, int);
extern short data_02082214[];
extern void* data_ov006_02137630[];
extern void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);

void func_ov006_021104c0(char* self)
{
    if (*(unsigned char*)(self + 0x30) == 0) return;
    {
        int f38 = *(int*)(self + 0x38);
        if (f38 > 0) {
            int scale;
            int idx = 1;
            int b, a;
            struct { int _00, _01, _10, _11; } vec;
            if (f38 < 0x20) {
                scale = _ZN4cstd4fdivEii(0x20000, f38 << 12);
            } else {
                scale = 0x1000;
                if ((f38 - 0x20) % 16 < 4) idx = 0;
            }
            b = (int)((((long long)data_02082214[1] * scale) + 0x800) >> 12);
            a = (int)((((long long)data_02082214[0] * scale) + 0x800) >> 12);
            vec._00 = b;
            vec._01 = a;
            vec._10 = -a;
            vec._11 = b;
            func_ov004_020b0104(data_ov006_02137630[idx], *(int*)(self + 8) >> 12, *(int*)(self + 0xc) >> 12, -1, 0, &vec);
            func_ov004_020b0104(data_ov006_02137630[2], *(int*)(self + 8) >> 12, (*(int*)(self + 0xc) >> 12) + 5, -1, 2, &vec);
        } else {
            if (*(unsigned char*)(self + 0x31) != 0) {
                func_ov004_020afdd0(data_ov006_02137630[0], *(int*)(self + 8) >> 12, *(int*)(self + 0xc) >> 12, -1, 0);
                func_ov004_020afdd0(data_ov006_02137630[2], *(int*)(self + 8) >> 12, (*(int*)(self + 0xc) >> 12) + 5, -1, 2);
            } else {
                func_ov004_020afdd0(data_ov006_02137630[1], *(int*)(self + 8) >> 12, *(int*)(self + 0xc) >> 12, -1, 0);
                func_ov004_020afdd0(data_ov006_02137630[2], *(int*)(self + 8) >> 12, (*(int*)(self + 0xc) >> 12) + 5, -1, 2);
            }
        }
    }
}