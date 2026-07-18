extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern void func_02017ab4(void);
extern void func_02017984(void);
extern void func_020072c0(void);
extern int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);

extern void* data_ov085_021305d8;
extern void* data_ov085_021305e8;
extern void* data_ov085_021305e0;
extern void* data_ov085_021305f4;
extern void* data_ov085_021305b8;
extern void* data_ov085_02130600;
extern void* data_ov085_021305d0;
extern void* data_ov085_0213060c;
extern void* data_ov085_021305b0;
extern void* data_ov085_02130618;
extern void* data_ov085_021305c8;
extern void* data_ov085_02130624;
extern void* data_ov085_021305c0;
extern void* data_ov085_02130630;

typedef struct { int x, y, z; } V3;
extern V3 data_ov085_021306ec;
extern void* data_ov085_0213063c;
extern V3 data_ov085_021306f8;
extern void* data_ov085_02130648;
extern V3 data_ov085_02130704;
extern void* data_ov085_02130654;
extern V3 data_ov085_02130710;
extern void* data_ov085_02130660;

typedef struct { int a, b; } S8;
typedef struct { S8 a, b; } S16;

extern S8 data_ov085_02130074, data_ov085_02130044;
extern S8 data_ov085_02130084, data_ov085_02130094;
extern S8 data_ov085_021300b4, data_ov085_0213003c;
extern S8 data_ov085_0213005c, data_ov085_02130064;
extern S8 data_ov085_0213007c, data_ov085_0213008c;
extern S8 data_ov085_02130054, data_ov085_0213009c;
extern S8 data_ov085_0213006c, data_ov085_0213004c;
extern S8 data_ov085_021300ac, data_ov085_021300a4;

extern S16 data_ov085_021306cc;
extern S16 data_ov085_0213066c;
extern S16 data_ov085_0213067c;
extern S16 data_ov085_0213069c;
extern S16 data_ov085_021306ac;
extern S16 data_ov085_021306bc;
extern S16 data_ov085_021306dc;
extern S16 data_ov085_0213068c;

extern int data_ov085_021305ac;

void __sinit_ov085_0212f5ec(void)
{
    func_02017acc(&data_ov085_021305d8, 0x452);
    func_020731dc(&data_ov085_021305d8, (void*)&func_02017ab4, (void**)&data_ov085_021305e8);
    func_02017acc(&data_ov085_021305e0, 0x3a4);
    func_020731dc(&data_ov085_021305e0, (void*)&func_02017ab4, (void**)&data_ov085_021305f4);
    _ZN13SharedFilePtr9ConstructEj(&data_ov085_021305b8, 0x3a5);
    func_020731dc(&data_ov085_021305b8, (void*)&func_02017984, (void**)&data_ov085_02130600);
    _ZN13SharedFilePtr9ConstructEj(&data_ov085_021305d0, 0x3a6);
    func_020731dc(&data_ov085_021305d0, (void*)&func_02017984, (void**)&data_ov085_0213060c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov085_021305b0, 0x3a7);
    func_020731dc(&data_ov085_021305b0, (void*)&func_02017984, (void**)&data_ov085_02130618);
    _ZN13SharedFilePtr9ConstructEj(&data_ov085_021305c8, 0x3a8);
    func_020731dc(&data_ov085_021305c8, (void*)&func_02017984, (void**)&data_ov085_02130624);
    _ZN13SharedFilePtr9ConstructEj(&data_ov085_021305c0, 0x3a9);
    func_020731dc(&data_ov085_021305c0, (void*)&func_02017984, (void**)&data_ov085_02130630);

    data_ov085_021306ec.x = 0x1c000;
    data_ov085_021306ec.y = -0x2a000;
    data_ov085_021306ec.z = 0x18000;
    func_020731dc(&data_ov085_021306ec, (void*)&func_020072c0, (void**)&data_ov085_0213063c);

    { volatile V3* p = (volatile V3*)&data_ov085_021306f8; p->x = 0x1c000; p->y = -0x14000; p->z = 0x32000; }
    func_020731dc(&data_ov085_021306f8, (void*)&func_020072c0, (void**)&data_ov085_02130648);

    { volatile V3* p = (volatile V3*)&data_ov085_02130704; p->x = 0x2b000; p->y = -0x2a000; p->z = 0x18000; }
    func_020731dc(&data_ov085_02130704, (void*)&func_020072c0, (void**)&data_ov085_02130654);

    { volatile V3* p = (volatile V3*)&data_ov085_02130710; p->x = 0x30000; p->y = -0xa000; p->z = 0x32000; }
    func_020731dc(&data_ov085_02130710, (void*)&func_020072c0, (void**)&data_ov085_02130660);

    data_ov085_021306cc.a = data_ov085_02130074;
    data_ov085_021306cc.b = data_ov085_02130044;
    data_ov085_0213066c.a = data_ov085_02130084;
    data_ov085_0213066c.b = data_ov085_02130094;
    data_ov085_0213067c.a = data_ov085_021300b4;
    data_ov085_0213067c.b = data_ov085_0213003c;
    data_ov085_0213069c.a = data_ov085_0213005c;
    data_ov085_0213069c.b = data_ov085_02130064;
    data_ov085_021306ac.a = data_ov085_0213007c;
    data_ov085_021306ac.b = data_ov085_0213008c;
    data_ov085_021306bc.a = data_ov085_02130054;
    data_ov085_021306bc.b = data_ov085_0213009c;
    data_ov085_021306dc.a = data_ov085_0213006c;
    data_ov085_021306dc.b = data_ov085_0213004c;
    data_ov085_0213068c.a = data_ov085_021300ac;
    data_ov085_0213068c.b = data_ov085_021300a4;

    data_ov085_021305ac = _ZN8SaveData22NumGlowingRabbitsFoundEv();
}
