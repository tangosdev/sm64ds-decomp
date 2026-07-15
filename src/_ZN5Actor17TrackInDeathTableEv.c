extern void DeathTable_SetBit(int id);

void _ZN5Actor17TrackInDeathTableEv(void *self)
{
    DeathTable_SetBit((short)((short *)self)[0xce >> 1]);
}
