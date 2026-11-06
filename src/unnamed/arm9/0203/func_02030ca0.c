extern int func_0203da9c(void);
extern int func_0203d950(int i);
extern int func_0203d890(void);
extern void func_02020214(unsigned int flags);

extern unsigned char data_0208f274;
extern unsigned char data_0209fc5c[];
extern signed char data_0209fc64[];
extern unsigned char data_0209fc50;
extern int data_0209fc68;

/* Session state 3's per-frame handler (data_020927ec[3]): mark the first
   data_0208f274 slots present if the link reports them (func_0203d950).
   An absent one clears slot 1, not its own slot, as the ROM does. Then
   rank the present slots, and once their count matches the player count
   the parent posts (func_0203d890) move to state 4 and post their mask
   (func_02020214). */
void func_02030ca0(void) {
    int count;
    int i;
    signed char cnt;
    int k;
    unsigned char mask;

    func_0203da9c();
    count = data_0208f274;
    for (i = 0; i < count; i++) {
        if (func_0203d950(i))
            data_0209fc5c[i] = 1;
        else
            data_0209fc5c[1] = 0;
    }

    cnt = 0;
    mask = 0;
    for (k = 0; k < 4; k++) {
        if (data_0209fc5c[k] != 0) {
            data_0209fc64[k] = cnt;
            cnt = cnt + 1;
            mask |= (1 << k);
        } else {
            data_0209fc64[k] = -1;
        }
    }

    data_0209fc50 = cnt;
    if (data_0209fc50 != func_0203d890())
        return;
    if (data_0209fc68 != 0)
        data_0209fc68 = 4;
    func_02020214(mask);
}
