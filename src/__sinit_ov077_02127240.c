extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern int func_02017934(void*, int);
extern void func_02017ab4(void);
extern void func_02017984(void);
extern void func_0201791c(void);
extern void func_020072c0(void);

extern void* data_ov077_02127b50;
extern void* data_ov077_02127b94;
extern void* data_ov077_02127b48;
extern void* data_ov077_02127ba0;
extern void* data_ov077_02127b38;
extern void* data_ov077_02127bac;
extern void* data_ov077_02127b28;
extern void* data_ov077_02127bb8;
extern void* data_ov077_02127b40;
extern void* data_ov077_02127b58;
extern void* data_ov077_02127b20;
extern void* data_ov077_02127b64;
extern void* data_ov077_02127b30;
extern void* data_ov077_02127b70;

struct P2 { int a, b; };
extern struct P2 data_ov077_02127bc4[6];
extern struct P2 data_ov077_021277e0;
extern struct P2 data_ov077_02127828;
extern struct P2 data_ov077_02127820;
extern struct P2 data_ov077_021277e8;
extern struct P2 data_ov077_02127818;
extern struct P2 data_ov077_02127800;
extern struct P2 data_ov077_021277f0;
extern struct P2 data_ov077_02127808;
extern struct P2 data_ov077_02127810;
extern struct P2 data_ov077_021277f8;

typedef struct { int x, y, z; } V3;
extern V3 data_ov077_02127b88;
extern void* data_ov077_02127b7c;

void __sinit_ov077_02127240(void)
{
    func_02017acc(&data_ov077_02127b50, 0x34c);
    func_020731dc(&data_ov077_02127b50, (void*)&func_02017ab4, (void**)&data_ov077_02127b94);
    func_02017acc(&data_ov077_02127b48, 0x429);
    func_020731dc(&data_ov077_02127b48, (void*)&func_02017ab4, (void**)&data_ov077_02127ba0);
    func_02017acc(&data_ov077_02127b38, 0x42a);
    func_020731dc(&data_ov077_02127b38, (void*)&func_02017ab4, (void**)&data_ov077_02127bac);
    _ZN13SharedFilePtr9ConstructEj(&data_ov077_02127b28, 0x34f);
    func_020731dc(&data_ov077_02127b28, (void*)&func_02017984, (void**)&data_ov077_02127bb8);
    _ZN13SharedFilePtr9ConstructEj(&data_ov077_02127b40, 0x34d);
    func_020731dc(&data_ov077_02127b40, (void*)&func_02017984, (void**)&data_ov077_02127b58);
    func_02017934(&data_ov077_02127b20, 0x350);
    func_020731dc(&data_ov077_02127b20, (void*)&func_0201791c, (void**)&data_ov077_02127b64);
    func_02017934(&data_ov077_02127b30, 0x34e);
    func_020731dc(&data_ov077_02127b30, (void*)&func_0201791c, (void**)&data_ov077_02127b70);

    data_ov077_02127bc4[0] = data_ov077_021277e0;
    data_ov077_02127bc4[1] = data_ov077_02127828;
    data_ov077_02127bc4[2] = data_ov077_02127820;
    data_ov077_02127bc4[3] = data_ov077_021277e8;
    data_ov077_02127bc4[4] = data_ov077_02127818;
    data_ov077_02127bc4[5] = data_ov077_02127800;
    data_ov077_02127bc4[6] = data_ov077_021277f0;
    data_ov077_02127bc4[7] = data_ov077_02127808;
    data_ov077_02127bc4[8] = data_ov077_02127810;
    data_ov077_02127bc4[9] = data_ov077_021277f8;

    data_ov077_02127b88.x = 0;
    data_ov077_02127b88.y = -0x1e000;
    data_ov077_02127b88.z = 0;
    func_020731dc(&data_ov077_02127b88, (void*)&func_020072c0, (void**)&data_ov077_02127b7c);
}
