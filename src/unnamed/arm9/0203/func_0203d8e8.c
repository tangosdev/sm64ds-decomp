extern int data_020a0f70;
/* Raise data_020a0f70, the flag the mode-3 link step (func_0203e20c)
   waits on in steps 3 and 7; func_0203e9a8 raises it on parent state 12.
   dScEntry_c reaches it through the veneer func_02030a6c. */
void func_0203d8e8(void) {
    data_020a0f70 = 1;
}
