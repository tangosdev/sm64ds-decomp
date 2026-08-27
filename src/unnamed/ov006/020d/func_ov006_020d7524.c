/* func_ov006_020d7524 at 0x020d7524
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern unsigned char* data_ov006_0213bb18[];
extern unsigned char* data_ov006_0213bb28[];
extern void* data_ov006_0213bb4c[];
typedef struct { int f660; int f664; char pad08[0x2c]; unsigned char b694; unsigned char b695; unsigned char b696; unsigned char b697; unsigned char pad38; unsigned char b699; unsigned char pad3a; unsigned char b69b; char pad3c[4]; } E;
typedef struct { char pad0[0x4660]; E e[112]; char pad1[0x7c]; int f62dc; } Self;
void func_ov006_020d7524(Self* self) {
    int i;
    for (i = 0; i < 0x70; i++) {
        if (self->e[i].b699 == 0) continue;
        {
            int b694 = self->e[i].b694;
            int a4 = 1;
            int b695, b696;
            unsigned char idx;
            int b697;
            int f664;
            int a1, a2;
            if (b694 == 3) a4 = 0;
            b696 = self->e[i].b696;
            b695 = self->e[i].b695;
            if (b696 != 0) { idx = data_ov006_0213bb28[b694][b695]; }
            else { idx = data_ov006_0213bb18[b694][b695]; }
            b697 = self->e[i].b697;
            a1 = self->e[i].f660 >> 12;
            f664 = self->e[i].f664;
            a2 = f664 >> 12;
            if (b697 == 6) {
                if (self->e[i].b69b == 4) { a2 = (f664 - self->f62dc) >> 12; }
            }
            func_ov004_020afdd0(data_ov006_0213bb4c[idx], a1, a2, -1, a4);
        }
    }
}
