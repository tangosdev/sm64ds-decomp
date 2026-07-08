extern signed char NUM_LIVES;

void GiveLives(int delta)
{
    int v = NUM_LIVES + delta;
    if (v < 0)
        v = 0;
    else if (v > 0x64)
        v = 0x64;
    NUM_LIVES = (signed char)v;
}
