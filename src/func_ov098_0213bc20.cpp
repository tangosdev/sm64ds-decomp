//cpp
extern "C" {
extern int func_ov098_0213b6e0(char* c);
extern void func_ov098_0213b584(char* c);
extern void func_ov098_0213b63c(char* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* b);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* a, void* b, unsigned int j);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int data_ov098_0213c930[];

int func_ov098_0213bc20(char* c) {
    if (func_ov098_0213b6e0(c)) {
        func_ov098_0213b584(c);
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        return 1;
    }

    int idx = *(int*)(c + 0x3c4);
    char* ent = (char*)&data_ov098_0213c930[idx * 2];
    int adj = *(int*)(ent + 4);
    char* self = c + (adj >> 1);
    void* fn;
    if (adj & 1) {
        void* vt = *(void**)self;
        fn = *(void**)((char*)vt + *(int*)ent);
    } else {
        fn = *(void**)ent;
    }
    ((void (*)(char*))fn)(self);

    if (*(int*)(c + 0x3c8) != 0) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);
        if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x144)) {
            if (*(int*)(c + 0x3c8) == 1) {
                func_ov098_0213b63c(c);
            } else {
                _ZN9ActorBase18MarkForDestructionEv(c);
            }
            return 0;
        }
        func_ov098_0213b584(c);
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
    } else {
        DecIfAbove0_Short((unsigned short*)(c + 0x100));
    }
    return 1;
}
}
