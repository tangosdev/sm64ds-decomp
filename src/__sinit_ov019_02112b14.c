extern void func_020731dc(void*, void*, void*);
extern int data_ov019_021135d8[];
extern int func_020072c0[];
extern int data_ov019_021135cc[];

void __sinit_ov019_02112b14(void) {
    *(unsigned int*)data_ov019_021135d8 = 0xfed60000;
    *(unsigned int*)((char*)data_ov019_021135d8 + 4) = 0x700000;
    *(unsigned int*)((char*)data_ov019_021135d8 + 8) = (unsigned int)-0x200000;
    func_020731dc(data_ov019_021135d8, func_020072c0, data_ov019_021135cc);
}
