extern int DP_STATE[];
extern int data_ov075_0211d828[];
extern int data_ov075_0211d780[];

extern void func_ov075_0211a148(void *c, void *data, int size);

void func_ov075_02116c38(void *c) {
    if (*DP_STATE) {
        func_ov075_0211a148(c, data_ov075_0211d828, 0x14);
    } else {
        func_ov075_0211a148(c, data_ov075_0211d780, 0x14);
    }
}
