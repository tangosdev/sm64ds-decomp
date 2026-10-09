unsigned int OS_GetInitArenaLo(int id)
{
    switch (id) {
    case 0: return 0x214eaa0;
    case 2: return 0x2400000;
    case 3: return 0x1ffdf40;
    case 4: return 0x23c0020;
    case 5: return 0x27ff000;
    case 6: return 0x37f8000;
    }
    return 0;
}
