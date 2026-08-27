struct S{char b[0x10000];};
void func_ov006_020d604c(struct S*c){
  *(unsigned char*)((char*)c+0x62ad)=1;
  *(unsigned char*)((char*)c+0x62ac)=0;
  *(unsigned char*)((char*)c+0x62ae)=0;
  *(unsigned char*)((char*)c+0x62af)=1;
  *(short*)((char*)c+0x62a8)=0;
  *(int*)((char*)c+0x62a0)=0x80000;
  *(int*)((char*)c+0x62a4)=0x8000;
}
