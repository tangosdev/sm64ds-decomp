extern int data_ov004_020beb68[];
void func_ov004_020adc00(int v){
  int r1=data_ov004_020beb68[0];
  if(r1!=0){
    *(int*)(r1+0x4000+0x650)=v;
  }
}
