extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int func_02017ab4[];
extern int SharedFilePtr_Destruct_Clsn[];

extern int data_ov098_0213c850[];
extern int data_ov098_0213c860[];
extern int data_ov098_0213c858[];
extern int data_ov098_0213c86c[];

struct P { int a, b; };
extern struct P data_ov098_0213c4a8;
extern struct P data_ov098_0213c488;
extern struct P data_ov098_0213c490;
extern struct P data_ov098_0213c468;
extern struct P data_ov098_0213c498;
extern struct P data_ov098_0213c470;
extern struct P data_ov098_0213c4b0;
extern struct P data_ov098_0213c480;
extern struct P data_ov098_0213c4a0;
extern struct P data_ov098_0213c478;
extern struct P data_ov098_0213c458;
extern struct P data_ov098_0213c460;
extern struct P data_ov098_0213c4c0;
extern struct P data_ov098_0213c4b8;
extern struct P data_ov098_0213c878[14];

void __sinit_ov098_0213c058(void)
{
    func_02017acc(data_ov098_0213c850, 0x458);
    func_020731dc(data_ov098_0213c850, func_02017ab4, data_ov098_0213c860);
    func_02017b4c(data_ov098_0213c858, 0x459);
    func_020731dc(data_ov098_0213c858, SharedFilePtr_Destruct_Clsn, data_ov098_0213c86c);

    data_ov098_0213c878[0] = data_ov098_0213c4a8;
    data_ov098_0213c878[1] = data_ov098_0213c488;
    data_ov098_0213c878[2] = data_ov098_0213c490;
    data_ov098_0213c878[3] = data_ov098_0213c468;
    data_ov098_0213c878[4] = data_ov098_0213c498;
    data_ov098_0213c878[5] = data_ov098_0213c470;
    data_ov098_0213c878[6] = data_ov098_0213c4b0;
    data_ov098_0213c878[7] = data_ov098_0213c480;
    data_ov098_0213c878[8] = data_ov098_0213c4a0;
    data_ov098_0213c878[9] = data_ov098_0213c478;
    data_ov098_0213c878[10] = data_ov098_0213c458;
    data_ov098_0213c878[11] = data_ov098_0213c460;
    data_ov098_0213c878[12] = data_ov098_0213c4c0;
    data_ov098_0213c878[13] = data_ov098_0213c4b8;
}
