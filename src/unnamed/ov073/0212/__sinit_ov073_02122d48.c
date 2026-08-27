extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Clsn(void);

extern char data_ov073_02123428[];
extern char data_ov073_02123494[];
extern char data_ov073_02123430[];
extern char data_ov073_02123470[];
extern char data_ov073_02123440[];
extern char data_ov073_0212347c[];
extern char data_ov073_02123448[];
extern char data_ov073_02123488[];
extern char data_ov073_02123450[];
extern char data_ov073_02123458[];
extern char data_ov073_02123438[];
extern char data_ov073_02123464[];

struct P2 { int a, b; };
extern struct P2 data_ov073_021234b0[];
extern struct P2 data_ov073_02123134;
extern struct P2 data_ov073_0212312c;
extern struct P2 data_ov073_021234c0[];
extern struct P2 data_ov073_0212311c;
extern struct P2 data_ov073_0212310c;
extern struct P2 data_ov073_021234d0[];
extern struct P2 data_ov073_02123114;
extern struct P2 data_ov073_02123124;
extern struct P2 data_ov073_021234a0[];
extern struct P2 data_ov073_02123144;
extern struct P2 data_ov073_0212313c;

void __sinit_ov073_02122d48(void)
{
    func_02017acc(data_ov073_02123428, 0x5e7);
    func_020731dc(data_ov073_02123428, func_02017ab4, data_ov073_02123494);
    func_02017acc(data_ov073_02123430, 0x5e9);
    func_020731dc(data_ov073_02123430, func_02017ab4, data_ov073_02123470);
    func_02017acc(data_ov073_02123440, 0x5eb);
    func_020731dc(data_ov073_02123440, func_02017ab4, data_ov073_0212347c);
    func_02017b4c(data_ov073_02123448, 0x5e8);
    func_020731dc(data_ov073_02123448, SharedFilePtr_Destruct_Clsn, data_ov073_02123488);
    func_02017b4c(data_ov073_02123450, 0x5ea);
    func_020731dc(data_ov073_02123450, SharedFilePtr_Destruct_Clsn, data_ov073_02123458);
    func_02017b4c(data_ov073_02123438, 0x5ec);
    func_020731dc(data_ov073_02123438, SharedFilePtr_Destruct_Clsn, data_ov073_02123464);

    data_ov073_021234b0[0] = data_ov073_02123134;
    data_ov073_021234b0[1] = data_ov073_0212312c;
    data_ov073_021234c0[0] = data_ov073_0212311c;
    data_ov073_021234c0[1] = data_ov073_0212310c;
    data_ov073_021234d0[0] = data_ov073_02123114;
    data_ov073_021234d0[1] = data_ov073_02123124;
    data_ov073_021234a0[0] = data_ov073_02123144;
    data_ov073_021234a0[1] = data_ov073_0212313c;
}
