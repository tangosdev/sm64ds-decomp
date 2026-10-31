/* One 0x24-byte player slot of data_020a1154 (see func_0203df40's record). */
struct E17 {
    char _f0[4];
    int touchSample; /* 0x04 -- the slot's TPData (nitro/spi/tp.h) */
    char _rest[0x24 - 8];
};
extern struct E17 data_020a1154[];
/* daPly_c idx's touch sample (x, y, touch, validity at +0x4); func_0203bb60
   and the DSMT scene read it. */
int *func_0203dabc(int idx) {
    return &data_020a1154[idx].touchSample;
}
