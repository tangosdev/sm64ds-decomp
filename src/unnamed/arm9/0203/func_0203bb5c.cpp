//cpp
/* Empty. Boot (func_0201a054) calls it right after the touch-panel setup
   (func_0203bbc0). */
extern "C" void func_0203bb5c(void)
{
}

/* One file-scope object with an empty inline constructor. mwcc emits the
   4-byte bx lr at 0x020750b4. The word is padding before data_020a0df8,
   so nothing later moves. */
struct EmptyInit {
    int unused;
    EmptyInit() {}
};

EmptyInit data_020a0df0;
