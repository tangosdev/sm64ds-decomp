extern int data_02086fcc;

struct Obj {
    char _pad[0x13c];
    int field_13c;
};

void func_0200cb58(struct Obj *obj, int index) {
    obj->field_13c = index * 0x28 + (int)&data_02086fcc;
}
