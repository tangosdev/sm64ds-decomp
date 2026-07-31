//cpp
extern "C" {
typedef int (*FP)(void*);
extern FP data_ov007_02103258;
int AllocMem(int a, void* b){
  return data_ov007_02103258(b);
}
}
