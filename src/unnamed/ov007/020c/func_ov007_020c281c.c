/* func_ov007_020c281c at 0x020c281c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern int _ZN4cstd3divEii(int a, int b);
extern int func_ov007_020c2dfc(char* self, int idx, int v);
extern int func_ov007_020c24d0(char* self, int idx);

void func_ov007_020c281c(char* self, int arg1) {
    int i;
    if (*(unsigned short*)(self + 8) < 3) return;
    _ZN4cstd3divEii(arg1 << 16, 0x168);
    switch (*(int*)self) {
    case 0:
        for (i = 2; i < *(unsigned short*)(self + 8); i++) {
            if (func_ov007_020c2dfc(self, i, arg1) != 0) {
                func_ov007_020c24d0(self, i - 1);
            }
        }
        break;
    case 1:
        break;
    }
}
