//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct Obj {
  virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3();
  virtual int v4(); virtual int v5(); virtual int v6(); virtual int v7();
  virtual int v8(); virtual int v9(); virtual int v10(); virtual int v11();
  virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
  virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19(char* c);
};

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern int _ZN6Player7IsStateERNS_5StateE(char* c, void* s);
extern void func_ov002_020c9e40(char* c);
extern void func_ov002_020d7430(char* c);
extern void func_ov002_020d8118(char* c);

extern char data_ov002_02110034;
extern char data_ov002_0211013c;
extern char data_ov002_02110154;
extern char data_ov002_0211067c;
extern char data_ov002_0211004c;

int func_ov002_020d6790(char* self)
{
    if (*(u16*)(self + 0x6c6) != 0) return 0;

    Obj* o = *(Obj**)(self + 0x360);
    if (o == 0) return 0;

    int isbf = (*(u16*)((char*)o + 0xc) == 0xbf);
    if (isbf) {
        if (*(u8*)(self + 0x709) != 0) goto isbf_ret0;
        if (*(u8*)(self + 0x708) != 0) goto isbf_ret0;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110034);
        func_ov002_020c9e40(self);
        return 1;
isbf_ret0:
        return 0;
    }

    if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c)) {
        if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)) {
            if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211067c)) return 0;
        }
    }

    o = *(Obj**)(self + 0x360);
    switch (o->v18()) {
    case 2:
    case 4:
        func_ov002_020d7430(self);
        goto ret0;
    case 5: {
        int eq132 = (*(u16*)((char*)(*(Obj**)(self + 0x360)) + 0xc) == 0x132);
        if (eq132) {
            func_ov002_020d8118(self);
        }
        (*(Obj**)(self + 0x360))->v19(self);
        *(int*)(self + 0x360) = 0;
        *(u8*)(self + 0x6f4) = 1;
        goto ret0;
    }
    case 3:
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110034);
        return 1;
    case 1:
    case 6:
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211004c);
        return 1;
    case 7:
        if (*(u8*)(self + 0x714) != 0) goto case7_ret1;
        *(int*)(((long long)(int)(*(int*)(self + 0x360) + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40000;
        *(int*)(((long long)(int)(*(int*)(self + 0x360) + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20000;
        *(u8*)(self + 0x714) = 1;
case7_ret1:
        return 1;
    default:
ret0:
        return 0;
    }
}
}
