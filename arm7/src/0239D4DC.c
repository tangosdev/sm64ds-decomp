/* storage 0x0239D4DC runtime 0x027CF454 size 0x44 main ARM. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
void fn_02395CB8(int a1) /* =0x027C7C30 */;
void fn_0239462C(void) /* =0x027C65A4 */;

int fn_0239D4DC(int a1, int a2) {
    unsigned int v2;
    *(unsigned short *)(a2 + 2) = 1;
    v2 = *(unsigned short *)(unk_380FFF4 + 784);
    if (v2 >= 0x20)
        fn_02395CB8(v2);
    fn_0239462C();
    return 0;
}
