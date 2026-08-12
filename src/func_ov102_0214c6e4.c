// @symbol func_ov102_0214c6e4
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
int func_ov102_0214c6e4(unsigned char *p)
{
    return p[263] == 0;
}
