//cpp
extern "C" {
extern int func_ov032_02111254(void*);
extern int func_ov032_02111ff4(void*, void*);
extern int data_ov032_02113aac[];
extern int data_ov032_02113a8c[];
int func_ov032_02111d7c(void* c){
  if(func_ov032_02111254(c)==1){ func_ov032_02111ff4(c, data_ov032_02113aac); return 1; }
  if(*(unsigned short*)((char*)c+0x100)==0){ func_ov032_02111ff4(c, data_ov032_02113a8c); }
  return 1;
}
}
