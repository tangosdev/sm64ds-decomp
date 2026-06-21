/* func_ov002_020e6350 at 0x020e6350
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern unsigned char data_0209f2d8;
extern unsigned char data_ov002_020ff0f4[];
extern void func_02011ee4(int c);
void func_ov002_020e6350(int *p) {
    int b = (data_0209f2d8 == 1);
    switch(b){case 0: break; default: return;}
    func_02011ee4(data_ov002_020ff0f4[p[2] & 3]);
}
