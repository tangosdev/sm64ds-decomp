extern int fn_02383940(int);

unsigned fn_023838C8(int value) {
    int result;
    int shift;
    if (value < -723)
        value = -723;
    else if (value > 0)
        value = 0;
    result = fn_02383940(value + 723);
    if (value < -240)
        shift = 3;
    else if (value < -120)
        shift = 2;
    else if (value < -60)
        shift = 1;
    else
        shift = 0;
    return (unsigned short)(result | (shift << 8));
}
