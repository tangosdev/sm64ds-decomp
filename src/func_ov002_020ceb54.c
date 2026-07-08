extern int WATER_HEIGHT;

int func_ov002_020ceb54(char *p)
{
    return WATER_HEIGHT - 0x50000 >= *(int *)(p + 0x60) + 0x3c000;
}
