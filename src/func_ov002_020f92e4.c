// @symbol func_ov002_020f92e4
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
int func_ov002_020f92e4(char *p)
{
    unsigned char b = *(unsigned char *)(p + 0x36d);
    if (b != 0 && b != 4)
        return 5;
    return 0;
}
