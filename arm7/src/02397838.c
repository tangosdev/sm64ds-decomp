extern void fn_02395BE0(int a1, int a2);

int fn_02397838(unsigned int a1, unsigned int a2) {
    if (a1 > 3)
        return 5;
    if (a2 > 0x3F)
        return 5;
    fn_02395BE0(0x13, (int)a1);
    fn_02395BE0(0x35, (int)a2);
    return 0;
}
