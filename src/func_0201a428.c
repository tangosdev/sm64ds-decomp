/* func_0201a428 at 0x0201a428
 * Destroys a heap referenced by global pointer and nulls the pointer.
 */

extern void *data_0209d524; /* global: void* (Heap*) */
extern void _ZN4Heap7DestroyEv(void *);

void func_0201a428(void) {
    _ZN4Heap7DestroyEv(data_0209d524);
    data_0209d524 = 0;
}
