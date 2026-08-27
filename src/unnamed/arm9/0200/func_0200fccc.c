struct T{int a;int b;int c;};
extern int func_0200fc44(char*,struct T*,int);
int func_0200fccc(char*s,int r1){
struct T t={*(int*)(s+0x5c),*(int*)(s+0x60),*(int*)(s+0x64)};
return func_0200fc44(s,&t,r1);
}
