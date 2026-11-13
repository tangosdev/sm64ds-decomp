extern unsigned char data_0209fc50;
extern signed char data_0209fc64[];
extern unsigned char data_0209fc5c[];

/* Seat n players for a local game (SetNumPlayers): slots 0..n-1 are
   present (data_0209fc5c) and ranked in order (data_0209fc64), the rest
   absent with rank -1, and data_0209fc50 holds the count. */
void func_020308d0(int n)
{
    int i;
    data_0209fc50 = n;
    for (i = 0; i < n; i++) {
        data_0209fc64[i] = i;
        data_0209fc5c[i] = 1;
    }
    for (i = n; i < 4; i++) {
        data_0209fc64[i] = -1;
        data_0209fc5c[i] = 0;
    }
}
