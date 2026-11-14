/* fn_02388B80: init the mailbox tables and the SPI register pair. */

extern unsigned short word_3809424[];
extern int unk_3809450[];

extern int fn_023829B0(void);
extern void fn_023829C0(void);
extern void fn_0238299C(int *);
extern int fn_023825DC(int *, int);
extern void fn_02388CD8(int);

int fn_02388B80(void)
{
    int v2;
    int v4 = 0x54505641;
    int result;

    ((int *)word_3809424)[8] = 0;
    ((int *)word_3809424)[10] = 30;
    ((int *)word_3809424)[9] = 5;
    v2 = 0;
    do {
        word_3809424[v2] = 0;
        ++v2;
    } while (v2 < 16);
    if (fn_023829B0() == 0)
        fn_023829C0();
    v2 = 0;
    do {
        fn_0238299C(&unk_3809450[8 * v2]);
        fn_023825DC(&unk_3809450[8 * v2], v4);
        ++v2;
    } while (v2 < 4);
    while (*(volatile unsigned short *)0x40001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x40001C0 = 0x8A01;
    *(volatile unsigned short *)0x40001C2 = 0x84;
    while (*(volatile unsigned short *)0x40001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x40001C2 = 0;
    while (*(volatile unsigned short *)0x40001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x40001C0 = 0x8201;
    *(volatile unsigned short *)0x40001C2 = 0;
    while ((result = *(volatile unsigned short *)0x40001C0 & 0x80) != 0)
        fn_02388CD8(1);
    return result;
}
