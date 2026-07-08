extern unsigned char NUM_PLAYERS;
extern signed char NUM_VS_STARS_OBTAINED_PLAYER;
extern signed char data_0209f311;

signed char NumVsStarsObtained(void)
{
    int i = 1;
    signed char sum = NUM_VS_STARS_OBTAINED_PLAYER;
    signed char *p;
    if (i >= (int)NUM_PLAYERS) return sum;
    p = &data_0209f311;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)NUM_PLAYERS);
    return sum;
}
