extern void DeathTable_SetBit(int);

void _ZN5Actor17TrackInDeathTableEv(char *self)
{
    DeathTable_SetBit(*(short *)(self + 0xce));
}
