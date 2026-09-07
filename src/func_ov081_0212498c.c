extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
extern void func_02012694(int, char*);
int func_ov081_0212498c(char *c){
 int id=*(int*)(c+0x3fc);
 if(id!=0){
   char *a=(char*)_ZN8dActor_c10FindWithIDEj(id);
   if(a!=0){
     *(int*)(a+0x9c) = -0x2000;
     *(int*)(a+0xa0) = -0x28000;
     *(int*)(c+0x3fc)=0;
   }
 }
 func_02012694(0xdb, c+0x74);
 *(short*)(c+0x414)=0;
 *(int*)(c+0x410)=0;
 return 1;
}
