extern void DeathTable_ClearBit(int);

void _ZN5Actor19UntrackInDeathTableEv(char *self)
{
    DeathTable_ClearBit(*(short *)(self + 0xce));
}
