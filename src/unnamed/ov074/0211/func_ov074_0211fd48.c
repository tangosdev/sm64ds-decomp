void func_ov074_0211fd48(char *c)
{
    unsigned char value;

    (*(unsigned char *)(((int)c + 0x604)))--;
    value = *(unsigned char *)(c + 0x604);
    if (value > 5)
        value = 5;
    *(unsigned char *)(c + 0x604) = value;
}
