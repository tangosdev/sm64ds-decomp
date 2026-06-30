/* func_ov007_020bcba8 at 0x020bcba8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void* func_ov007_020b3f20(int n);
extern void* func_02054ea8(void);
extern unsigned int func_02054e88(void);
extern int func_02054d88(void);
extern void func_ov007_020b8548(int a0, void* a1, int a2, void* a3, unsigned int a4, int a5, int a6);
extern void _ZN2GX7LoadOBJEPKvjj(const void*, unsigned int, unsigned int);
extern void func_ov007_020c32e8(char* c, int sel, unsigned int n, int off);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void*, unsigned int, unsigned int);
extern void func_ov007_020b3edc(int r0);
extern void func_02056014(const void* src, unsigned int offset, unsigned int count);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void*, unsigned int, unsigned int);
extern void func_ov007_020bcdb8(int idx);
extern void func_ov007_020c33d0(char* c, int sel, unsigned int off);
extern void func_ov007_020b850c(void);
extern char* data_ov007_0210342c;

void func_ov007_020bcba8(int idx)
{
    char* g = data_ov007_0210342c;
    char* s = *(char**)(g + 0x74);

    switch (idx) {
    case 0:
        return;
    case 1:
        {
            void* a1 = func_ov007_020b3f20(0x25);
            void* a3 = func_02054ea8();
            unsigned int a4 = func_02054e88();
            int a6 = *(int*)(*(char**)(*(char**)(g + 0x28)) + 0x9c);
            int idx2;
            func_ov007_020b8548(0, a1, 0, a3, a4, 0, a6);

            _ZN2GX7LoadOBJEPKvjj(*(void**)(*(char**)(*(char**)(s + 0x24) + 4)), 0, 0x2000);
            func_ov007_020c32e8(*(char**)(*(char**)(s + 0x2c) + 4), 0, 0, 0);
            _ZN2GX7LoadOBJEPKvjj(func_ov007_020b3f20(0x2c), 0x2000, 0x2000);
            _ZN2GX11LoadOBJPlttEPKvjj((char*)func_ov007_020b3f20(0x2d) + 0xc0, 0x1e0, 0x20);
            func_ov007_020b3edc(0x2c);
            func_ov007_020b3edc(0x2d);

            idx2 = *(unsigned char*)(*(char**)(data_ov007_0210342c + 0x28) + 0x3a) + 0x25;
            func_02056014(func_ov007_020b3f20(idx2), 0, 0x4000);
            _ZN2GX10LoadBGPlttEPKvjj(func_ov007_020b3f20(0x2b), 0, 0x200);
            func_ov007_020b3edc(idx2);
            func_ov007_020b3edc(0x2b);

            func_ov007_020bcdb8(1);
            func_ov007_020c33d0(*(char**)(*(char**)(s + 0x24)), 8, 0);
            func_ov007_020c32e8(*(char**)(*(char**)(s + 0x2c)), 3, 0, 0);
            func_ov007_020b850c();
        }
        break;
    case 3:
        {
            void* a1 = func_ov007_020b3f20(0x25);
            void* a3 = func_02054ea8();
            unsigned int a4 = func_02054e88();
            int a5 = func_02054d88();
            int a6 = *(int*)(*(char**)(*(char**)(g + 0x28)) + 0x9c);
            func_ov007_020b8548(0, a1, 0, a3, a4, a5, a6);

            func_ov007_020bcdb8(2);
            func_ov007_020c33d0(*(char**)(*(char**)(s + 0x24) + 0xc), 9, 0);
            func_ov007_020c32e8(*(char**)(*(char**)(s + 0x2c) + 8), 3, 0, 0);
            func_ov007_020b850c();
        }
        break;
    case 2:
        break;
    }
}
