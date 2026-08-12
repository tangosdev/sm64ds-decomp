// @symbol func_ov020_021127cc
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
int func_ov020_021127cc(char* c){
  unsigned int b = *(unsigned short*)(c+0xc)==0x147; return b ? 2 : 0;
}
