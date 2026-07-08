/* func_ov006_0211102c at 0x0211102c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern short SINE_TABLE[];
extern void* data_ov006_02138a08[];
extern void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, void* a5);

void func_ov006_0211102c(char* self)
{
    int b = (short)((((long long)SINE_TABLE[(*(unsigned short*)(self + 0x32) >> 4) * 2 + 1] << 12) + 0x800) >> 12);
    int a;
    int vec[4];

    vec[0] = b;
    a = (short)((((long long)SINE_TABLE[(*(unsigned short*)(self + 0x32) >> 4) * 2] << 12) + 0x800) >> 12);
    vec[3] = b;
    vec[1] = a;
    vec[2] = -a;

    func_ov004_020b0104(data_ov006_02138a08[0], *(int*)(self + 8) >> 12, *(int*)(self + 0xc) >> 12, -1, 1, vec);
    func_ov004_020b0104(data_ov006_02138a08[1], *(int*)(self + 8) >> 12, (*(int*)(self + 0xc) >> 12) + 5, -1, 2, vec);
}
