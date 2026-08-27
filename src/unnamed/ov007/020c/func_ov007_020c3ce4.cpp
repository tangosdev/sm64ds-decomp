//cpp
extern "C" {
typedef int (*FP)(void);
extern FP data_ov007_02104c14;
int func_ov007_020c3ce4(void){
  if (data_ov007_02104c14 == 0) return 0;
  return data_ov007_02104c14();
}
}
