extern int DeathTable_GetBit(int);

int _ZN5Actor18GetBitInDeathTableEv(char *self)
{
    return DeathTable_GetBit(*(short *)(self + 0xce));
}
