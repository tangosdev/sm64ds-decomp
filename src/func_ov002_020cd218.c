/* func_ov002_020cd218 at 0x020cd218
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern short data_02082214[];

void func_ov002_020cd218(char* self, int r1, int* outA, int* outB)
{
    int idx = (*(unsigned short*)(self + 0x92)) >> 4;
    *outA = (int)(((long long)r1 * data_02082214[idx * 2 + 1] + 0x800) >> 12);
    int idx2 = (*(unsigned short*)(self + 0x92)) >> 4;
    *outB = -(int)(((long long)r1 * data_02082214[idx2 * 2] + 0x800) >> 12);
}
