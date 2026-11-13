extern int data_0209fc68;
extern unsigned char data_0209fc5c[];
extern signed char data_0209fc64[];
extern unsigned char data_0209fc50;
extern unsigned char data_0209fc58;

/* Re-rank the present slots once a session is running: each present slot
   (data_0209fc5c) gets the next rank in data_0209fc64, an absent one -1.
   The count goes to data_0209fc50, the disconnect bits data_0209fc58 are
   cleared, and the session state data_0209fc68 moves to 4. dScEntry_c
   calls it when the players are settled. */
void func_02030994(void)
{
    signed char acc;
    int i;
    unsigned char* src;
    signed char* dst;
    if (data_0209fc68 == 0) return;
    acc = 0;
    src = data_0209fc5c;
    dst = data_0209fc64;
    i = 0;
    do {
        unsigned char b = *src;
        i++;
        src++;
        if (b != 0) {
            *dst = acc;
            acc = (acc + 1);
        } else {
            *dst = -1;
        }
        dst++;
    } while (i < 4);
    data_0209fc50 = acc;
    data_0209fc58 = 0;
    data_0209fc68 = 4;
}
