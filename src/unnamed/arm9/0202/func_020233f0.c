extern char data_0209ee90[];

/* Byte 0x345 of data_0209ee90: LoadDebugFont sets it through func_02023408
   once the font is in BG1; func_020233f4 clears it (dScBoot_c, dScTitle_c and
   the minigame scene call it while setting up their screens). No reader of
   the byte is in src/ yet. func_020233f0 is empty; func_0201a054 calls it. */
void func_02023408(void) {
    data_0209ee90[0x345] = 1;
}

void func_020233f4(void) {
    data_0209ee90[0x345] = 0;
}

void func_020233f0(void) {}
