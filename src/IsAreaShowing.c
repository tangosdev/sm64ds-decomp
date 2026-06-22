struct Area { char pad[4]; unsigned char flag; char pad2[7]; };
extern struct Area *data_0209f314;
unsigned char IsAreaShowing(int idx) {
    return data_0209f314[idx].flag;
}
