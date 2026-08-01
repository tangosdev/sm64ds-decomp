extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern int SharedFilePtr_Construct_TexSeq(void*, int);
extern void* data_ov060_0211b208;
extern void func_02017ab4(void);
extern void* data_ov060_0211b228;
extern void* data_ov060_0211b1f8;
extern void SharedFilePtr_Destruct_Anim(void);
extern void* data_ov060_0211b21c;
extern void* data_ov060_0211b200;
extern void SharedFilePtr_Destruct_TexSeq(void);
extern void* data_ov060_0211b210;
void __sinit_ov060_0211a428(void)
{
    func_02017acc(&data_ov060_0211b208, 0x384);
    func_020731dc(&data_ov060_0211b208, (void*)&func_02017ab4, (void**)&data_ov060_0211b228);
    _ZN13SharedFilePtr9ConstructEj(&data_ov060_0211b1f8, 0x383);
    func_020731dc(&data_ov060_0211b1f8, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov060_0211b21c);
    SharedFilePtr_Construct_TexSeq(&data_ov060_0211b200, 0x385);
    func_020731dc(&data_ov060_0211b200, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov060_0211b210);
}
