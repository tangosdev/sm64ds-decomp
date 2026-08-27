//cpp
struct Foo {
    char padding[0x31c];
    int field31c;
    int arr[20];
    int field370;
    int field374;
};
extern "C" int func_ov064_021197fc(Foo *c) {
    int i = 0;
    c->field31c = i;
    c->field370 = i;
    c->field374 = i;
    for (int v = i; i < 20; i++) {
        c->arr[i] = v;
    }
    return 1;
}
