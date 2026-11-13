extern void _Z13func_0203d8d4v(void);
extern int data_0209fc68;
extern unsigned char gSoftResetDisabled;

/* Ask the mode-3 link step to wind down (func_0203d8d4 sets
   data_020a0f40, which func_0203e20c reads), then
   clear the session state data_0209fc68 and the skip flag gSoftResetDisabled. */
void func_02030a38(void)
{
    _Z13func_0203d8d4v();
    data_0209fc68 = 0;
    gSoftResetDisabled = 0;
}
