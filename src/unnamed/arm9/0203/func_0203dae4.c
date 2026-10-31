/* data_020a1154 holds the four players' 0x24-byte input records (func_0203df40's
 * Rec). data_020a1162 is the first record's keys field (0x0e), so a view one
 * record long, starting there, steps to player idx's. */
struct RecKeys {
    unsigned short keys; /* Rec 0x0e -- the buttons held this frame */
    char rest[0x24 - 2];
};
extern struct RecKeys data_020a1162[];

/* daPly_c idx's held buttons (func_0203bc7c and the DSMT scene read them). */
int func_0203dae4(int idx) {
    return data_020a1162[idx].keys;
}
