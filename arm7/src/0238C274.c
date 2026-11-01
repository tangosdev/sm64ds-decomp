/* storage 0x0238C274 runtime 0x0380410C size 0x2C wram. No pool. */

extern void fn_0238C098(void);
extern void fn_02382B38(void);

void fn_0238C274(int unused, int flags) {
    if ((flags & 0x3F) == 2)
        fn_0238C098();
    else
        fn_02382B38();
}
