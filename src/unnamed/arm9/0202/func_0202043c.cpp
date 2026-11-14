//cpp
/* Restart an OamAnimation at frame 0 (func_0202044c).
 * sOamRestartInit exists so this file emits the empty initializer at
 * 0x02074e80 (bx lr, no relocations), between the RNG initializer and
 * d_s_stage_methods.cpp. The object is deadstripped: the initializer never
 * references it, and a .bss byte after Timer.cpp would shift every later symbol.
 */
extern "C" int func_0202044c(char *a, int b);

int func_0202043c(void *a) {
    return func_0202044c((char *)a, 0);
}

struct OamRestartInit {
    OamRestartInit() {}
};
static OamRestartInit sOamRestartInit;
