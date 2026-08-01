extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int func_02017b4c(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Clsn(void);

extern void* data_ov095_02137898;
extern void* data_ov095_02137904;
extern void* data_ov095_021378a8;
extern void* data_ov095_021378c8;
extern void* data_ov095_021378c0;
extern void* data_ov095_021378d4;
extern void* data_ov095_021378b0;
extern void* data_ov095_021378e0;
extern void* data_ov095_021378b8;
extern void* data_ov095_021378ec;
extern void* data_ov095_021378a0;
extern void* data_ov095_021378f8;

struct S2 { int w[2]; };
extern struct S2 data_ov095_02137594;
extern struct S2 data_ov095_0213757c;
extern struct S2 data_ov095_02137584;
extern struct S2 data_ov095_0213758c;
extern struct S2 data_ov095_0213759c;
struct D { struct S2 a, b, c, d, e; };
extern struct D data_ov095_02137910;

void __sinit_ov095_0213722c(void)
{
    func_02017acc(&data_ov095_02137898, 0x6bf);
    func_020731dc(&data_ov095_02137898, (void*)&func_02017ab4, (void**)&data_ov095_02137904);
    func_02017acc(&data_ov095_021378a8, 0x5db);
    func_020731dc(&data_ov095_021378a8, (void*)&func_02017ab4, (void**)&data_ov095_021378c8);
    func_02017acc(&data_ov095_021378c0, 0x6ab);
    func_020731dc(&data_ov095_021378c0, (void*)&func_02017ab4, (void**)&data_ov095_021378d4);
    func_02017b4c(&data_ov095_021378b0, 0x6c0);
    func_020731dc(&data_ov095_021378b0, (void*)&SharedFilePtr_Destruct_Clsn, (void**)&data_ov095_021378e0);
    func_02017b4c(&data_ov095_021378b8, 0x5dc);
    func_020731dc(&data_ov095_021378b8, (void*)&SharedFilePtr_Destruct_Clsn, (void**)&data_ov095_021378ec);
    func_02017b4c(&data_ov095_021378a0, 0x6ac);
    func_020731dc(&data_ov095_021378a0, (void*)&SharedFilePtr_Destruct_Clsn, (void**)&data_ov095_021378f8);
    data_ov095_02137910.a = data_ov095_02137594;
    data_ov095_02137910.b = data_ov095_0213757c;
    data_ov095_02137910.c = data_ov095_02137584;
    data_ov095_02137910.d = data_ov095_0213758c;
    data_ov095_02137910.e = data_ov095_0213759c;
}
