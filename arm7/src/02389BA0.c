/* storage 0x02389BA0 runtime 0x03801A38 size 0x1A0 (code 0x188 + 6-word pool) wram ARM.
 * Display/sound init under lock fn_02382AC4/fn_02382AD8: saves MMIO 0x4000208
 * and halfword 0x4000134, runs file/sound/irq helpers, then restores both
 * and clears table word via unk_38094E8[8]. Three numeric pool words
 * (0x4000208, 0x4000132, 0x4000134) + three ABS32 data relocs. */

extern int fn_02382AC4(void); /* =0x037FA95C */
extern int fn_02382AD8(int); /* =0x037FA970 */
extern int fn_02380884(int); /* =0x037F871C */
extern int fn_02389898(int); /* =0x03801730 */
extern void fn_02389D60(int); /* =0x03801BF8 */
extern void fn_02389A04(int); /* =0x0380189C */
extern void fn_023833CC(void); /* =0x037FB264 */
extern void fn_02389848(int); /* =0x038016E0 */
extern void fn_023808C0(int); /* =0x037F8758 */
extern void fn_02383210(int); /* =0x037FB0A8 */
extern void fn_0238322C(int, int); /* =0x037FB0C4 */
extern void fn_02389D40(void); /* =0x03801BD8 */
extern void fn_02389958(int, int); /* =0x038017F0 */
extern void fn_02389874(int); /* =0x0380170C */
extern void fn_02383374(void); /* =0x037FB20C */
extern void fn_023897EC(int, int, int); /* =0x03801684 */
extern void fn_023808F8(int); /* =0x037F8790 */
extern unsigned short word_38094E0; /* =0x038094E0 */
extern unsigned short word_38094DC; /* =0x038094DC */
extern int unk_38094E8[]; /* =0x038094E8 */

int fn_02389BA0(void) {
    int v2;
    int v3;
    int v4;
    unsigned short v0;
    unsigned short v1;
    v1 = *(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 0;
    v2 = fn_02382AC4();
    v3 = fn_02380884(0x1FFFFFF);
    v4 = fn_02389898(0);
    fn_02389D60(2);
    fn_02389A04(2);
    fn_02389A04(2);
    fn_023833CC();
    fn_02389848(1);
    if ((word_38094E0 & 1) != 0) {
        *(volatile unsigned short *)0x4000132 = word_38094DC | 0x4000;
        fn_023808C0(0x1000);
    }
    if ((word_38094E0 & 4) != 0)
        fn_023808C0(0x400000);
    if ((word_38094E0 & 2) != 0) {
        v0 = *(volatile unsigned short *)0x4000134;
        fn_02383210(0x8000);
        fn_0238322C(64, 64);
        fn_0238322C(256, 256);
        fn_023808C0(0x80);
    }
    if ((word_38094E0 & 8) != 0)
        fn_023808C0(0x100000);
    if ((word_38094E0 & 0x10) != 0)
        fn_023808C0(0x2000);
    fn_02382AD8(v2);
    (void)*(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 1;
    fn_02389D40();
    fn_02389958(0, v4);
    *(volatile unsigned short *)0x4000134 = v0;
    fn_02389874(1);
    fn_02383374();
    unk_38094E8[8] = 0;
    fn_023897EC(98, 0, 0);
    fn_02382AC4();
    fn_023808F8(v3);
    fn_02382AD8(v2);
    {
        int result = *(volatile unsigned short *)0x4000208;
        *(volatile unsigned short *)0x4000208 = v1;
        return result;
    }
}
