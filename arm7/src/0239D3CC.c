/* storage 0x0239D3CC runtime 0x027CF344 size 0x60 main ARM. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
void fn_023965A8(void) /* =0x027C8520 */;
void fn_02382CC8(int a1, int a2, int a3) /* =0x037FAB60 */;

int fn_0239D3CC(int a1, int a2) {
    if (*(unsigned short *)(unk_380FFF4 + 784) == 0)
        return 1;
    *(unsigned short *)(a2 + 2) = 92;
    fn_023965A8();
    fn_02382CC8(unk_380FFF4 + 0x4F4, a2 + 8, 180);
    return 0;
}
