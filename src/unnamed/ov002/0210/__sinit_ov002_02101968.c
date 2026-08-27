extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();
extern char data_ov002_0210e12c[];
extern char data_ov002_0210e140[];
extern char data_ov002_0210e124[];
extern char data_ov002_0210e134[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Clsn(void);
void __sinit_ov002_02101968(void) {
    func_02017acc(data_ov002_0210e12c, 0x45f);
    func_020731dc(data_ov002_0210e12c, func_02017ab4, data_ov002_0210e140);
    func_02017b4c(data_ov002_0210e124, 0x460);
    func_020731dc(data_ov002_0210e124, SharedFilePtr_Destruct_Clsn, data_ov002_0210e134);
}
