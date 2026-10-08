extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern void* data_ov070_02123530;
extern void* data_ov070_02123544;
extern void* data_ov070_02123520;
extern void* data_ov070_02123550;
extern void* data_ov070_02123518;
extern void* data_ov070_0212355c;
extern void* data_ov070_02123510;
extern void* data_ov070_02123568;
extern void* data_ov070_02123528;
extern void* data_ov070_02123574;
extern void* data_ov070_02123508;
extern void* data_ov070_02123580;
extern void* data_ov070_02123500;
extern void* data_ov070_02123538;

struct P2 { int a, b; };
extern struct P2 data_ov070_0212359c[2];
extern struct P2 data_ov070_021230e8;
extern struct P2 data_ov070_021230c8;
extern struct P2 data_ov070_021235ac[2];
extern struct P2 data_ov070_021230d8;
extern struct P2 data_ov070_021230f0;
extern struct P2 data_ov070_021235cc[2];
extern struct P2 data_ov070_021230c0;
extern struct P2 data_ov070_02123118;
extern struct P2 data_ov070_021235dc[2];
extern struct P2 data_ov070_02123108;
extern struct P2 data_ov070_021230d0;
extern struct P2 data_ov070_0212358c[2];
extern struct P2 data_ov070_02123110;
extern struct P2 data_ov070_021230e0;
extern struct P2 data_ov070_021235bc[2];
extern struct P2 data_ov070_021230f8;
extern struct P2 data_ov070_02123100;

void __sinit_ov070_02122afc(void)
{
    func_02017acc(&data_ov070_02123530, 0x411);
    func_020731dc(&data_ov070_02123530, (void*)&func_02017ab4, (void**)&data_ov070_02123544);
    _ZN13SharedFilePtr9ConstructEj(&data_ov070_02123520, 0x417);
    func_020731dc(&data_ov070_02123520, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov070_02123550);
    _ZN13SharedFilePtr9ConstructEj(&data_ov070_02123518, 0x412);
    func_020731dc(&data_ov070_02123518, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov070_0212355c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov070_02123510, 0x413);
    func_020731dc(&data_ov070_02123510, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov070_02123568);
    _ZN13SharedFilePtr9ConstructEj(&data_ov070_02123528, 0x414);
    func_020731dc(&data_ov070_02123528, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov070_02123574);
    _ZN13SharedFilePtr9ConstructEj(&data_ov070_02123508, 0x415);
    func_020731dc(&data_ov070_02123508, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov070_02123580);
    _ZN13SharedFilePtr9ConstructEj(&data_ov070_02123500, 0x416);
    func_020731dc(&data_ov070_02123500, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov070_02123538);

    data_ov070_0212359c[0] = data_ov070_021230e8;
    data_ov070_0212359c[1] = data_ov070_021230c8;
    data_ov070_021235ac[0] = data_ov070_021230d8;
    data_ov070_021235ac[1] = data_ov070_021230f0;
    data_ov070_021235cc[0] = data_ov070_021230c0;
    data_ov070_021235cc[1] = data_ov070_02123118;
    data_ov070_021235dc[0] = data_ov070_02123108;
    data_ov070_021235dc[1] = data_ov070_021230d0;
    data_ov070_0212358c[0] = data_ov070_02123110;
    data_ov070_0212358c[1] = data_ov070_021230e0;
    data_ov070_021235bc[0] = data_ov070_021230f8;
    data_ov070_021235bc[1] = data_ov070_02123100;
}
