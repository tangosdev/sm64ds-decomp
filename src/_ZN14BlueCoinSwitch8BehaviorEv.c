typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern void _ZN5Event8ClearBitEj(u32 bit);
extern void _ZN5Event6SetBitEj(u32 bit);
extern void _ZN9ActorBase18MarkForDestructionEv(void* p);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);

int _ZN14BlueCoinSwitch8BehaviorEv(char* c)
{
    if (*(u16*)(c + 0x10a) != 0) {
        *(u16*)(((int)c + 0x10a) & 0xFFFFFFFFFFFFFFFF) -= 1;
        if (*(u16*)(c + 0x10a) == 0) {
            *(u32*)(((int)c + 0xec) & 0xFFFFFFFFFFFFFFFF) &= ~1;
            _ZN5Event8ClearBitEj(*(u8*)(c + 0x10d));
        }
    }
    if (*(int*)(c + 0xf8) != 0) {
        *(u32*)(((long long)(int)(c + 0xec)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        _ZN5Event6SetBitEj(*(u8*)(c + 0x10d));
        if (*(u8*)(c + 0x10c) != 0) {
            *(u16*)(c + 0x10a) = *(u16*)(c + 0x108);
        } else {
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
    }
    _ZN12CylinderClsn5ClearEv(c + 0xd4);
    _ZN12CylinderClsn6UpdateEv(c + 0xd4);
    return 1;
}
