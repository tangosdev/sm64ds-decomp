/* func_02044dc4 at 0x02044dc4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Self;
extern void func_02044e50(struct Self *self, int count, int base, int *out);

void func_02044dc4(struct Self *self) {
    unsigned int i;
    int r7;
    char *r6;
    r6 = *(char**)self;
    r7 = *(int*)(r6 + 4) * 0x30 + *(int*)((char*)self + 0xc);
    for (i = 0; i < *(unsigned int*)(r6 + 0x30); i++) {
        unsigned short *arr = *(unsigned short**)(*(char**)(r6 + 0x34) + 8);
        func_02044e50(self, arr[2*i], arr[2*i+1], (int*)r7);
        r7 += 0x30;
    }
}