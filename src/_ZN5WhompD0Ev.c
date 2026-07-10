extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN15TextureSequenceD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int func_ov002_020aed18(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int _ZTV5Whomp[];
extern void* data_020a0eac;
int _ZN5WhompD0Ev(char *c){
    *(int*)c=(int)_ZTV5Whomp;
    _ZN18MovingMeshColliderD1Ev(c+0x418);
    _ZN11ShadowModelD1Ev(c+0x344);
    _ZN15TextureSequenceD1Ev(c+0x330);
    _ZN9ModelAnimD1Ev(c+0x2cc);
    _ZN12WithMeshClsnD1Ev(c+0x110);
    func_ov002_020aed18(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac);
    return (int)c;
}
