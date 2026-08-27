extern void *_ZN4Heap14CreateRootHeapEPvj(void *start, unsigned int size);
extern char data_ov062_0211d9c0;
extern char func_02140d80;
extern void *data_0209d524;

void func_0201a458(void) {
    data_0209d524 = _ZN4Heap14CreateRootHeapEPvj(&data_ov062_0211d9c0, (unsigned int)(&func_02140d80 - &data_ov062_0211d9c0));
}
