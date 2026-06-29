extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern int func_02017934(void*, int);
extern void func_02017ab4(void);
extern void func_02017984(void);
extern void func_0201791c(void);

extern void* data_ov071_02123050;
extern void* data_ov071_02123058;
extern void* data_ov071_02123038;
extern void* data_ov071_02123064;
extern void* data_ov071_02123040;
extern void* data_ov071_02123070;
extern void* data_ov071_02123048;
extern void* data_ov071_0212307c;

struct S2 { int w[2]; };
extern struct S2 data_ov071_02122cb8;
extern struct S2 data_ov071_02122cc0;
extern struct S2 data_ov071_02122ca8;
extern struct S2 data_ov071_02122cd0;
extern struct S2 data_ov071_02122cb0;
extern struct S2 data_ov071_02122cc8;
struct D { struct S2 a, b, c, d, e, f; };
extern struct D data_ov071_02123088;

void __sinit_ov071_021228c8(void)
{
    func_02017acc(&data_ov071_02123050, 0x2f7);
    func_020731dc(&data_ov071_02123050, (void*)&func_02017ab4, (void**)&data_ov071_02123058);
    func_02017934(&data_ov071_02123038, 0x2f8);
    func_020731dc(&data_ov071_02123038, (void*)&func_0201791c, (void**)&data_ov071_02123064);
    func_02017934(&data_ov071_02123040, 0x2fb);
    func_020731dc(&data_ov071_02123040, (void*)&func_0201791c, (void**)&data_ov071_02123070);
    _ZN13SharedFilePtr9ConstructEj(&data_ov071_02123048, 0x2f9);
    func_020731dc(&data_ov071_02123048, (void*)&func_02017984, (void**)&data_ov071_0212307c);
    data_ov071_02123088.a = data_ov071_02122cb8;
    data_ov071_02123088.b = data_ov071_02122cc0;
    data_ov071_02123088.c = data_ov071_02122ca8;
    data_ov071_02123088.d = data_ov071_02122cd0;
    data_ov071_02123088.e = data_ov071_02122cb0;
    data_ov071_02123088.f = data_ov071_02122cc8;
}
