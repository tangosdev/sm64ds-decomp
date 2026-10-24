/* storage 0x0239D498 runtime 0x027CF410 size 0x44 main ARM. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
void fn_02396914(void) /* =0x027C888C */;

int fn_0239D498(int a1, int a2) {
    if (*(unsigned short *)(unk_380FFF4 + 784) == 0)
        return 1;
    *(unsigned short *)(a2 + 2) = 1;
    fn_02396914();
    return 0;
}
