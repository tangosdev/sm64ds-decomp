/* storage 0x023803C8 runtime 0x037F8260 size 0xA0 wram ARM.
 * No pool, ten BL calls (fn_02382B38 is AM9's). */

extern int fn_023819C4(int a1);
extern int fn_023819B0(int a1);
extern int fn_02381B24(int a1, int a2, int a3, int a4);
extern int fn_02382D2C(int a1, int a2, int a3);
extern int fn_023818C0(int a1, int a2);
extern int fn_02381A7C(int a1, int a2, int a3);
extern int fn_02381BCC(int a1, int a2);
extern void fn_02382B38(void);

int fn_023803C8(void) {
    int v0 = fn_023819C4(8);
    int v2 = fn_02381B24(8, fn_023819B0(8), v0, 1);
    int v3 = fn_023819C4(8);
    fn_02382D2C(v2, 0, v3 - v2);
    fn_023818C0(8, v2);
    v0 = fn_023819C4(8);
    v3 = fn_02381A7C(8, fn_023819B0(8), v0);
    if (v3 < 0)
        fn_02382B38();
    fn_02381BCC(8, v3);
    return v3;
}
