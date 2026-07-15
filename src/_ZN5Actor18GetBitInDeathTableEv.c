extern unsigned int DeathTable_GetBit(int id);

unsigned int _ZN5Actor18GetBitInDeathTableEv(void *self)
{
    return DeathTable_GetBit((short)((short *)self)[0xce >> 1]);
}
