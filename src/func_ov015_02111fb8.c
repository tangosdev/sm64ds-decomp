typedef void (*FnPtr)(void*);
struct VtEntry {
    int field0;
    int field1;
};
extern struct VtEntry data_ov015_02114a24[];

void func_ov015_02111fb8(void* self, int idx) {
    struct VtEntry* e = (struct VtEntry*)((char*)data_ov015_02114a24 + (idx << 3));
    int f1 = e->field1;
    void* obj = (void*)((char*)self + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1) {
        fn = (FnPtr)*(int*)((char*)*(int**)obj + e->field0);
    } else {
        fn = (FnPtr)e->field0;
    }
    fn(obj);
    *(int*)((char*)self + 0x330) = idx;
}
