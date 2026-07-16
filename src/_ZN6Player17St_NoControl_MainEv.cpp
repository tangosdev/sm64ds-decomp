//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

extern "C" {
extern int func_0200ee68(void);
extern short GetAngleToCamera(int i);
extern void func_ov002_020c965c(char* c);
extern int func_ov002_020c94a4(char* c);
extern void func_ov002_020c92fc(char* c);
extern void func_ov002_020c9288(char* c);
extern void func_ov002_020c924c(char* c);
extern int func_ov002_020c91bc(char* c);
extern void func_ov002_020c9718(char* c);
extern int func_ov002_020c9128(char* c);
extern void func_ov002_020c8cb0(char* c);
extern void func_ov002_020c904c(char* c);
extern void func_ov002_020c8f80(char* c);
extern void func_ov002_020c8f0c(char* c);
extern void func_ov002_020c8b54(char* c);
extern void func_ov002_020c8a4c(char* c);
extern void func_ov002_020c897c(char* c);
extern void func_ov002_020c8d14(char* c);
extern void func_ov002_020c8b78(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, unsigned int id, int flags, int speed, unsigned int extra);
extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* thiz, int f);
extern int _ZN6Player12FinishedAnimEv(void* thiz);
extern void func_ov002_020c8714(char* c);
extern int func_ov002_020c8540(char* c);
extern int func_ov002_020c84b0(char* c);
extern void func_ov002_020bedd4(char* c);

extern int data_ov002_0211013c[];
}

extern "C" int _ZN6Player17St_NoControl_MainEv(char* self)
{
    u8 mode = *(u8*)(self + 0x70a);
    if (mode == 0 || mode == 1 || (mode == 0x11 && *(u8*)(self + 0x6e3) != 0x18)) {
        if (func_0200ee68() == 0) {
            *(s16*)(self + 0x8e) = GetAngleToCamera(*(u8*)(self + 0x6d8));
            *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);
        }
    }

    switch (*(u8*)(self + 0x6e3)) {
    case 0:
        func_ov002_020c965c(self);
        break;
    case 1:
        if (func_ov002_020c94a4(self) != 0)
            return 1;
        break;
    case 2:
    case 9:
        func_ov002_020c92fc(self);
        break;
    case 10:
        func_ov002_020c9288(self);
        break;
    case 20:
        func_ov002_020c924c(self);
        return 1;
    case 21:
        if (func_ov002_020c91bc(self) != 0)
            return 1;
        break;
    case 3:
    case 6:
        if (*(u16*)(self + 0x6a6) == 0)
            *(int*)(self + 0x98) = 0;
        if (*(u8*)(self + 0x70a) == 6 && *(u8*)(self + 0x706) != 0) {
            *(int*)(self + 0x9c) = 0;
            *(int*)(self + 0xa8) = 0;
        } else {
            func_ov002_020c9718(self);
        }
        if (*(u8*)(self + 0x70a) == 0 || *(u8*)(self + 0x70a) == 3)
            return 1;
        break;
    case 4:
    case 5:
        if (func_ov002_020c9128(self) != 0)
            return 1;
        break;
    case 7:
    case 8:
        func_ov002_020c8cb0(self);
        break;
    case 11:
        func_ov002_020c904c(self);
        break;
    case 12:
        func_ov002_020c8f80(self);
        break;
    case 13:
        func_ov002_020c8f0c(self);
        break;
    case 14:
        func_ov002_020c8b54(self);
        break;
    case 15:
        func_ov002_020c8a4c(self);
        break;
    case 16:
        func_ov002_020c897c(self);
        break;
    case 17:
        func_ov002_020c8d14(self);
        break;
    case 18:
        func_ov002_020c8b78(self);
        break;
    case 19:
        if (*(u8*)(self + 0x70b) == 0)
            _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_0211013c);
        break;
    case 22:
        if (*(u8*)(self + 0x6de) == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x98, 0x40000000, 0x1000, 0);
            *(u8*)(self + 0x6e3) = 0x17;
        }
        break;
    case 23: {
        int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self + 8) & 0xff, 0);
        char* anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x1c))
            *(u8*)(self + 0x71a) = 0;
        if (_ZN6Player12FinishedAnimEv(self))
            _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_0211013c);
        break;
    }
    case 24:
        func_ov002_020c8714(self);
        break;
    case 25:
        if (func_ov002_020c8540(self) != 0)
            return 1;
        break;
    case 26:
        if (func_ov002_020c84b0(self) != 0)
            return 1;
        break;
    }

    func_ov002_020bedd4(self);
    return 1;
}
