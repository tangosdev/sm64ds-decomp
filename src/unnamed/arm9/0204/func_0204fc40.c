extern void _ZN18NestedHeapIteratorC1Ej(void *thiz, unsigned int n);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(void *thiz, void *ha);

struct E1 {
    unsigned char pad[0x2c];
    unsigned char f2c;
    unsigned char pad2[0xf];
    unsigned char f3c;
    unsigned char pad3[7];
};

struct E2 {
    unsigned char a[0xc];
    unsigned char b[0xc];
    int f18;
};

extern char data_020a4d60[];
extern char data_020a4d54[];
extern struct E1 data_020a50ec[];
extern struct E2 data_020a4d6c[];

void func_0204fc40(void)
{
    int i;
    _ZN18NestedHeapIteratorC1Ej(data_020a4d60, 0x14);
    _ZN18NestedHeapIteratorC1Ej(data_020a4d54, 0x14);
    for (i = 0; i < 0x10; i++) {
        data_020a50ec[i].f2c = 0;
        data_020a50ec[i].f3c = (unsigned char)i;
        _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(data_020a4d54, &data_020a50ec[i]);
    }
    for (i = 0; i < 0x20; i++) {
        _ZN18NestedHeapIteratorC1Ej(&data_020a4d6c[i], 0xc);
        _ZN18NestedHeapIteratorC1Ej(data_020a4d6c[i].b, 0);
        data_020a4d6c[i].f18 = 1;
    }
}
