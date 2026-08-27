int BookSwitch_Spawn(char *p)
{
    *(short *)(p + 0x100) = 100;
    *(int *)(p + 0x98) = 81920;
    return 1;
}
