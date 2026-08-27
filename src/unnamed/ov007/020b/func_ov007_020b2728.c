extern void func_ov007_020c421c(int x);
extern int* data_ov007_0210342c;
void func_ov007_020b2728(void){
  int i;
  for(i=0;i<3;i++){
    func_ov007_020c421c((data_ov007_0210342c+i)[0x108/4]);
  }
}
