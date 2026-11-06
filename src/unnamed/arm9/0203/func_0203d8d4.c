extern int data_020a0f40;
/* Raise data_020a0f40, which tells the mode-3 link step (func_0203e20c)
   to wind down. func_02030a38 calls it. */
void func_0203d8d4(void) {
    data_020a0f40 = 1;
}
