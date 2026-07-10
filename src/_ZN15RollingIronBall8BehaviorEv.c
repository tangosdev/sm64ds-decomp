typedef void (*FnPtr)(void*);
struct VtEntry {
    int field0;
    int field1;
};
extern struct VtEntry data_ov100_0214867c[];

int _ZN15RollingIronBall8BehaviorEv(void* self) {
    unsigned char idx = *(unsigned char*)((char*)self + 0x3d0);
    struct VtEntry* e = (struct VtEntry*)((char*)data_ov100_0214867c + ((int)idx << 3));
    int f1 = e->field1;
    void* obj = (void*)((char*)self + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1) {
        fn = (FnPtr)*(int*)((char*)*(int**)obj + e->field0);
    } else {
        fn = (FnPtr)e->field0;
    }
    fn(obj);
    return 1;
}
