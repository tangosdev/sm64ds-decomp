extern unsigned short func_02018a24(unsigned int);
void func_02017e0c(void *c, unsigned int arg){
  *(int*)((char*)c+4)=0;
  *(unsigned char*)((char*)c+2)=0;
  *(unsigned short*)c = func_02018a24(arg);
}
