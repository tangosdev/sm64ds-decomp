//cpp
// func_ov002_020b7d9c at 0x020b7d9c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* thiz);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* thiz);
extern int STAR_CAP_MIN_POS_Y;

int func_ov002_020b7d9c(char* c)
{
    if (STAR_CAP_MIN_POS_Y > *(int*)(c + 0x60)
        || _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144)
        || !DecIfAbove0_Short((unsigned short*)(c + 0x100))
        || (*(int*)(c + 0xb0) & 8))
    {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }
    return 1;
}
}
