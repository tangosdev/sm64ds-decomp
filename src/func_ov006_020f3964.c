void _ZN13dScMgMemory_c12UpdateCursorEv(char* c)
{
    if (*(unsigned char*)(c + 0x5000 + 0x310) == 0)
        return;

    {
        unsigned short* e = (unsigned short*)(c + 0x530c);
        *e = *e + 1;
        if (*e < 0x14)
            return;
        *e = 0;
    }
    {
        unsigned char* p = (unsigned char*)(c + 0x5311);
        *p = *p + 1;
        *p = *p & 1;
    }
}
