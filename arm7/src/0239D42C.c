/* storage 0x0239D42C runtime 0x027CF3A4 size 0x6C main ARM. */
extern char off_27D6A74[] /* =0x027D6A74 */;
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
void fn_02382C98(void *a1, void *a2, int a3) /* =0x037FAB30 */;
int fn_02395C74(int a1) /* =0x027C7BEC */;
int fn_02395B2C(void) /* =0x027C7AA4 */;

int fn_0239D42C(int a1, int a2) {
    *(unsigned short *)(a2 + 2) = 9;
    fn_02382C98(off_27D6A74, (void *)(a2 + 6), 8);
    *(unsigned short *)(a2 + 14) = *(unsigned short *)0x04808000;
    *(unsigned short *)(a2 + 16) = fn_02395C74(0);
    *(unsigned short *)(a2 + 18) = fn_02395B2C();
    *(unsigned short *)(a2 + 20) = *(unsigned short *)(unk_380FFF4 + 1456);
    return 0;
}
