/* storage 0x02387144 runtime 0x037FEFDC size 0x34 wram.
 * Pool +0x34: 03808744, 03808758, 038075C0. */

extern void fn_02381698(int, int, int);
extern void fn_02386C9C(void);
extern int dword_38075C0;

int *fn_02387144(void) {
    fn_02381698(0x03808744, 0x03808758, 8);
    fn_02386C9C();
    dword_38075C0 = 0;
    return &dword_38075C0;
}
