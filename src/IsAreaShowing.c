struct Area { char pad[4]; unsigned char flag; char pad2[7]; };
extern struct Area *AREAS;
unsigned char IsAreaShowing(int idx) {
    return AREAS[idx].flag;
}
