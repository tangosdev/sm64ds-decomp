unsigned short func_ov006_0212382c(int sel, int b)
{
    unsigned short r = 0;
    switch (sel) {
    case 0:
        if (b == 0) r |= 4;
        break;
    case 1:
        if (b == 0) r |= 0x60;
        else r |= 2;
        break;
    case 2:
        if (b == 0) r |= 0x48;
        break;
    case 3:
        if (b == 0) r |= 0x100;
        else r |= 0x28;
        break;
    case 4:
        if (b == 1) r |= 0x80;
        break;
    }
    return r;
}
