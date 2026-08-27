void func_ov007_020bc434(void* c){
  *(int*)((char*)c+0x3c)=0;
  *(int*)((char*)c+0x38)=*(int*)((char*)c+0x3c);
  *(int*)((char*)c+0x34)=*(int*)((char*)c+0x38);
  *(int*)((char*)c+0x48)=0x1000;
  *(int*)((char*)c+0x44)=*(int*)((char*)c+0x48);
  *(int*)((char*)c+0x40)=*(int*)((char*)c+0x44);
  *(unsigned short*)((char*)c+0x50)=0;
  *(unsigned short*)((char*)c+0x4e)=*(unsigned short*)((char*)c+0x50);
  *(unsigned short*)((char*)c+0x4c)=*(unsigned short*)((char*)c+0x4e);
}
