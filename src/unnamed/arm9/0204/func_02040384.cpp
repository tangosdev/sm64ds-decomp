//cpp
/* Empty: the WM indication callback func_0203fc38 installs. */
extern "C" void func_02040384(void) {}

/* One file-scope object with an empty inline constructor. mwcc emits the
   4-byte bx lr at 0x02075150. The word is padding before data_020a0df8,
   after func_0203bb5c's word, so nothing later moves. */
struct EmptyInit {
    int unused;
    EmptyInit() {}
};

EmptyInit data_020a0df4;
