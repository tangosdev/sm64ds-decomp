extern signed char data_0209f2f4;

void GiveLives(int delta)
{
    int v = data_0209f2f4 + delta;
    if (v < 0)
        v = 0;
    else if (v > 0x64)
        v = 0x64;
    data_0209f2f4 = (signed char)v;
}
