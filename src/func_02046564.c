struct S { int pad0; int a; char p8[0x24-8]; int c; char p28[0x30-0x28]; int b; };
struct A { char x[0x30]; };
int func_02046564(struct S *p){ int a=p->a; int b=p->b; int c=p->c;
 int t=(int)&((struct A*)0)[a+b]; t+=c*0x30; t+=a*0x34; return t; }
