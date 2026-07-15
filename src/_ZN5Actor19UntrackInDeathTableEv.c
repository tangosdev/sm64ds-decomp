extern void DeathTable_ClearBit(int id);

void _ZN5Actor19UntrackInDeathTableEv(void *self)
{
    DeathTable_ClearBit((short)((short *)self)[0xce >> 1]);
}
