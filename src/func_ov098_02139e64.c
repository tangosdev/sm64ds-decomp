// @symbol func_ov098_02139e64
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
int func_ov098_02139e64(unsigned char *c) {
  unsigned char v = c[0x606];
  if (v != 0) return 0;
  return 6;
}
