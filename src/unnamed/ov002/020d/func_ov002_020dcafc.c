/* func_ov002_020dcafc at 0x020dcafc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
typedef short s16;

extern int AngleDiff(int a, int b);
extern int ApproachAngle(void* p, int a, int b, int c, int d);
extern unsigned char data_020a0e40[];
extern short data_0209f4a2[];
extern short data_0209f4a4[];

void func_ov002_020dcafc(char* c) {
    s16 v5 = 0, v4 = 0;
    int idx;
    s16 a, b;

    if (*(int*)(c + 0x98) != 0) {
        idx = data_020a0e40[0] * 0x18;
        a = *(short*)((char*)data_0209f4a2 + idx);
        b = *(short*)((char*)data_0209f4a4 + idx);
        v5 = (s16)(((long long)a * 0xaab + 0x800) >> 12);
        v4 = (s16)(((long long)(-b) * 0x1555 + 0x800) >> 12);
    }

    if (AngleDiff(*(s16*)(c + 0x8e), *(s16*)(c + 0x94)) >= 0x4000) {
        v5 = (s16)(-v5);
        v4 = (s16)(-v4);
    }

    ApproachAngle(c + 0x75e, v5, 8, 0x200, 0x20);
    ApproachAngle(c + 0x760, v4, 8, 0x200, 0x20);
}
