extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void func_02017984(void);

extern char data_ov100_021489a4[];
extern char data_ov100_021489d4[];
extern char data_ov100_021489bc[];
extern char data_ov100_021489e0[];
extern char data_ov100_021489b4[];
extern char data_ov100_021489ec[];
extern char data_ov100_021489c4[];
extern char data_ov100_021489f8[];
extern char data_ov100_021489cc[];
extern char data_ov100_02148a04[];
extern char data_ov100_021489ac[];
extern char data_ov100_02148a10[];

struct P2 { int a, b; };
extern struct P2 data_ov100_02148a1c[];
extern struct P2 data_ov100_02148450;
extern struct P2 data_ov100_02148460;
extern struct P2 data_ov100_02148448;
extern struct P2 data_ov100_02148468;
extern struct P2 data_ov100_02148458;
extern struct P2 data_ov100_02148470;
extern struct P2 data_ov100_02148478;

void __sinit_ov100_02147bc0(void)
{
    func_02017acc(data_ov100_021489a4, 0x447);
    func_020731dc(data_ov100_021489a4, func_02017ab4, data_ov100_021489d4);
    func_02017acc(data_ov100_021489bc, 0x448);
    func_020731dc(data_ov100_021489bc, func_02017ab4, data_ov100_021489e0);
    func_02017acc(data_ov100_021489b4, 0x44a);
    func_020731dc(data_ov100_021489b4, func_02017ab4, data_ov100_021489ec);
    _ZN13SharedFilePtr9ConstructEj(data_ov100_021489c4, 0x44b);
    func_020731dc(data_ov100_021489c4, func_02017984, data_ov100_021489f8);
    _ZN13SharedFilePtr9ConstructEj(data_ov100_021489cc, 0x44c);
    func_020731dc(data_ov100_021489cc, func_02017984, data_ov100_02148a04);
    _ZN13SharedFilePtr9ConstructEj(data_ov100_021489ac, 0x449);
    func_020731dc(data_ov100_021489ac, func_02017984, data_ov100_02148a10);

    data_ov100_02148a1c[0] = data_ov100_02148450;
    data_ov100_02148a1c[1] = data_ov100_02148460;
    data_ov100_02148a1c[2] = data_ov100_02148448;
    data_ov100_02148a1c[3] = data_ov100_02148468;
    data_ov100_02148a1c[4] = data_ov100_02148458;
    data_ov100_02148a1c[5] = data_ov100_02148470;
    data_ov100_02148a1c[6] = data_ov100_02148478;
}
