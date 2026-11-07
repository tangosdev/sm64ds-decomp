/* storage 0x023A424C runtime 0x027D61C4 linked 0xF4 main ARM.
 * Read a length, allocate, then read the block. Each base access reloads
 * the pointer at 0x0380FFF4. */

extern int fn_0238853C(int handle);
extern void fn_023A43A0(void);
extern void fn_0238D084(int cmd, unsigned int len, void *dst);
extern void fn_023884D0(int handle);
extern int fn_02384E7C(void *heap, unsigned int size);

int fn_023A424C(void) {
    unsigned int len;
    int block;
    fn_0238853C(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2D8));
    fn_023A43A0();
    len = 0;
    fn_0238D084(0x2C, 2, &len);
    fn_023884D0(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2D8));
    if (len < 0xA4 || len > 0x1D6)
        return 0;
    len += 2;
    block = fn_02384E7C(*(unsigned char **)0x0380FFF4 + 0x14C, len);
    *(int *)(*(unsigned char **)0x0380FFF4 + 0x2DC) = block;
    block = *(int *)(*(unsigned char **)0x0380FFF4 + 0x2DC);
    if (block == 0)
        return 0;
    *(int *)(*(unsigned char **)0x0380FFF4 + 0x2DC) = block + 0xC;
    fn_0238853C(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2D8));
    fn_023A43A0();
    fn_0238D084(0x2A, len, *(void **)(*(unsigned char **)0x0380FFF4 + 0x2DC));
    fn_023884D0(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2D8));
    return 1;
}
