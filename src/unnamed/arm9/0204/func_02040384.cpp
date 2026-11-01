//cpp
/* Empty: the WM indication callback func_0203fc38 installs. */
extern "C" void func_02040384(void) {}

/* A file-scope object with an empty inline constructor. mwcc emits the
   4-byte bx lr at 0x02075150. The object stays static so the linker drops
   it: data_020a0df4 is padding between data_020a0df0 and data_020a0df8.
   Claiming that word here would list it after cstd_sqrt's .bss. */
struct EmptyInit {
    EmptyInit() {}
};
static EmptyInit sEmptyInit;
