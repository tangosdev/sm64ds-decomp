/* func_ov006_021117bc at 0x021117bc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern short SINE_TABLE[];
extern void* data_ov006_0213765c[];
extern void* data_ov006_02137674[];
extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern int _ZN4cstd4fdivEii(int, int);

void func_ov006_021117bc(char* self)
{
    switch (*(int*)(self + 0x34)) {
    case 0:
        func_ov004_020afdd0(data_ov006_0213765c[0], *(int*)(self + 8) >> 12, *(int*)(self + 0xc) >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_0213765c[1], *(int*)(self + 8) >> 12, (*(int*)(self + 0xc) >> 12) + 5, -1, 2);
        break;
    case 1:
        {
            int d;
            int b, a;
            int vec[4];
            if (*(int*)(self + 0x28) < 0x1000) return;
            d = _ZN4cstd4fdivEii(0x7000, *(int*)(self + 0x28));
            b = (int)(((long long)SINE_TABLE[1] * d + 0x800) >> 12);
            a = (int)(((long long)SINE_TABLE[0] * d + 0x800) >> 12);
            vec[0] = b;
            vec[1] = a;
            vec[2] = -a;
            vec[3] = b;
            func_ov004_020b0104(data_ov006_02137674[0], *(int*)(self + 8) >> 12, *(int*)(self + 0xc) >> 12, -1, 1, vec);
            func_ov004_020b0104(data_ov006_02137674[1], *(int*)(self + 8) >> 12, (*(int*)(self + 0xc) >> 12) + 5, -1, 2, vec);
        }
        break;
    }
}
