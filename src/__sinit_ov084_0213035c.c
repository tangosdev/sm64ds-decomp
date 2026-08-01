extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern char data_ov084_02130cf8[];
extern char data_ov084_02130d14[];
extern char data_ov084_02130d00[];
extern char data_ov084_02130d20[];
extern char data_ov084_02130ce8[];
extern char data_ov084_02130d2c[];
extern char data_ov084_02130ce0[];
extern char data_ov084_02130d38[];
extern char data_ov084_02130cd0[];
extern char data_ov084_02130d44[];
extern char data_ov084_02130cc0[];
extern char data_ov084_02130d50[];
extern char data_ov084_02130cd8[];
extern char data_ov084_02130d5c[];
extern char data_ov084_02130cc8[];
extern char data_ov084_02130d68[];
extern char data_ov084_02130cf0[];
extern char data_ov084_02130d08[];

struct P2 { int a, b; };
extern struct P2 data_ov084_02130d74[];
extern struct P2 data_ov084_021308bc;
extern struct P2 data_ov084_021308b4;
extern struct P2 data_ov084_021308c4;
extern struct P2 data_ov084_021308ac;
extern struct P2 data_ov084_021308cc;

void __sinit_ov084_0213035c(void)
{
    func_02017acc(data_ov084_02130cf8, 0x386);
    func_020731dc(data_ov084_02130cf8, func_02017ab4, data_ov084_02130d14);
    func_02017acc(data_ov084_02130d00, 0x38e);
    func_020731dc(data_ov084_02130d00, func_02017ab4, data_ov084_02130d20);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130ce8, 0x38d);
    func_020731dc(data_ov084_02130ce8, SharedFilePtr_Destruct_Anim, data_ov084_02130d2c);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130ce0, 0x388);
    func_020731dc(data_ov084_02130ce0, SharedFilePtr_Destruct_Anim, data_ov084_02130d38);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130cd0, 0x38a);
    func_020731dc(data_ov084_02130cd0, SharedFilePtr_Destruct_Anim, data_ov084_02130d44);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130cc0, 0x38b);
    func_020731dc(data_ov084_02130cc0, SharedFilePtr_Destruct_Anim, data_ov084_02130d50);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130cd8, 0x387);
    func_020731dc(data_ov084_02130cd8, SharedFilePtr_Destruct_Anim, data_ov084_02130d5c);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130cc8, 0x38c);
    func_020731dc(data_ov084_02130cc8, SharedFilePtr_Destruct_Anim, data_ov084_02130d68);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130cf0, 0x389);
    func_020731dc(data_ov084_02130cf0, SharedFilePtr_Destruct_Anim, data_ov084_02130d08);

    data_ov084_02130d74[0] = data_ov084_021308bc;
    data_ov084_02130d74[1] = data_ov084_021308b4;
    data_ov084_02130d74[2] = data_ov084_021308c4;
    data_ov084_02130d74[3] = data_ov084_021308ac;
    data_ov084_02130d74[4] = data_ov084_021308cc;
}
