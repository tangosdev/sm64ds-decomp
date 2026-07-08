extern unsigned char SAVE_DATA[];
int func_020138dc(void){
  int count = 0;
  int i;
  int f = *(int*)(SAVE_DATA+0xc);
  for(i = 0; i < 0x1c; i++){
    if(f & (1 << i)) count++;
  }
  return count;
}
