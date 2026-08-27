/* func_ov007_020c0078 at 0x020c0078 (ov007)
 *
 * Camera-style perspective setup: writes the viewport register (0x4000580)
 * from four byte fields, then calls G3i::PerspectiveW_ with a sine-table FOV
 * pair and an aspect taken from a 64-bit fixed field at +0xbc shifted down
 * by 20, and finally clears 0x400044c.
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef int Fix12;
struct Matrix4x3;

extern void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    Fix12, int, int, int, int, int, int, struct Matrix4x3*);
extern short data_02082214[];

void func_ov007_020c0078(char* c)
{
    int a;
    *(int*)0x4000580 = *(unsigned char*)(c + 0xb8)
                     | (*(unsigned char*)(c + 0xba) << 8)
                     | (*(unsigned char*)(c + 0xb9) << 16)
                     | (*(unsigned char*)(c + 0xbb) << 24);

    a = (int)*(unsigned short*)(c + 0xd4) >> 4;
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        data_02082214[a * 2],
        data_02082214[a * 2 + 1],
        (int)(*(long long*)(c + 0xbc) >> 0x14),
        *(int*)(c + 0xc4),
        *(int*)(c + 0xc8),
        0x1000,
        1,
        (struct Matrix4x3*)(c + 0x74));

    *(int*)0x400044c = 0;
}
