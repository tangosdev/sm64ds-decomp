extern short BUTTON_INPUT_ARR[];

void func_0203bc50(void)
{
    short *p = BUTTON_INPUT_ARR;
    int i;
    for (i = 0; i < 4; i++) {
        p[0] = 0;
        p[1] = 0;
        p += 2;
    }
}
