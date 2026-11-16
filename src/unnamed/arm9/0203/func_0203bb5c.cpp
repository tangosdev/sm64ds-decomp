//cpp
/* Empty. Boot (func_0201a054) calls it right after the touch-panel setup
   (func_0203bbc0). */
void func_0203bb5c(void) {}

/* A file-scope object with an empty inline constructor. mwcc emits the
   4-byte bx lr at 0x020750b4. The object stays static so the linker drops
   it: the cartridge has no storage for it. This file sits among the touch
   functions, and their .bss from data_020a0dd8 to 0x020a0e40 is all live
   touch state, gTouchInfo's four records included. */
struct EmptyInit {
    EmptyInit() {}
};

static EmptyInit sEmptyInit;
