// @symbol func_ov006_020f5b98
// recovered name: dScMgMemory2_c_DrawCards
/* dScMgMemory2_c's card draw: the sixth call vtable slot 9 (Render) makes,
   from src/func_ov006_020f73f4.c.

   TWENTY records at +0x51a8, stride 0x18. src/func_ov006_020f6c90.c fills
   16, 18 or 20 of them at state 1 depending on the difficulty byte +0x540a,
   each with a card identity in 1..8, 1..9 or 1..10 dealt twice over;
   src/func_ov006_020f6088.c walks them from x = 128.0, y = -128.0 to their
   slot, and src/func_ov006_020f5de0.c and _020f5e74.c drive the flip frame.

   Per record:
     +0x12  nonzero when the card is in play (this loop's whole gate)
     +0x00  screen x, 20.12, shifted down by 12 for the sprite
     +0x04  screen y, 20.12
     +0x10  card identity, the ROW of the twelve-by-five halfword table at
            data_ov006_0213d45c
     +0x15  flip frame 0..4, its COLUMN

   The halfword the table yields indexes data_ov006_02142490, the twenty-three
   OAM pointers __sinit_ov006_021314e4 copies out of data_ov006_02133810.

   The fifth argument is the object's state index at +0x53d4, as a flag: every
   state but 2 passes 1. */

extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern unsigned short data_ov006_0213d45c[];
extern void *data_ov006_02142490[];

void func_ov006_020f5b98(char *c)
{
    char *p;
    int i;
    int flag;

    p = c;
    for (i = 0; i < 0x14; i++) {
        if (*(unsigned char *)(p + 0x51ba) != 0) {
            flag = 0;
            if (*(int *)(c + 0x53d4) != 2) flag = 1;
            Hud_RenderSprite(data_ov006_02142490[data_ov006_0213d45c[
                                 *(unsigned char *)(p + 0x51b8) * 5 +
                                 *(unsigned char *)(p + 0x51bd)]],
                             *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12,
                             -1, flag);
        }
        p += 0x18;
    }
}
