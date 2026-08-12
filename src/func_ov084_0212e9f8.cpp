//cpp
// @symbol func_ov084_0212e9f8
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" int func_ov084_0212e9f8(void *c) {
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int r;
    if (v == 0xfc) r = 1; else r = 0;
    if (r != 0) r = 4; else r = 0;
    return r;
}
