extern unsigned char data_020a0f04;
/* Set the link mode data_020a0f04 (func_0203df40 runs the link step for it:
   0 local only, 1 and 2 func_0203ea5c, 3 func_0203e20c). */
void func_0203da2c(int v) {
    data_020a0f04 = v;
}
