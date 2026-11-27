/* storage 0x023A43A0 runtime 0x027D6318 linked 0x38 main ARM.
 * Poll until bit 1 of the word fn_0238D1FC writes comes back clear.
 * The incoming argument is ignored. */

extern void fn_0238D1FC(unsigned int *out);

void fn_023A43A0(void) {
    unsigned int value = 2;
    while (value & 2)
        fn_0238D1FC(&value);
}
