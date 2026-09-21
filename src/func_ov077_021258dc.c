// @symbol func_ov077_021258dc
/* State 3 entry handler: PMF record data_ov077_021278e8 is copied into
 * data_ov077_02127c28 at +0x30 by __sinit_ov077_0212749c. */
extern void _ZN5dCc_c5ClearEv(void *);
int func_ov077_021258dc(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x1b0);
    *(int *)(c + 0x3d8) = 3;
    return 1;
}
