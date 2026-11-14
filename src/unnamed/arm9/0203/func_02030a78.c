extern void _Z13func_0203da2ci(int v);
extern int data_0209fc68;

/* Leave the session: local link mode (func_0203da2c(0)) and session state
   0. dScEntry_c and dScStage_c call it once a disconnect is handled. */
void func_02030a78(void)
{
    _Z13func_0203da2ci(0);
    data_0209fc68 = 0;
}
