extern int numCommonModelData[];
extern int commonModelDataArr[];
extern int data_0208e738[];
extern int _ZN5Model13LoadTexAndPalER8BMD_File(void*);

void* _ZN5Model23AddToCommonModelDataArrER8BMD_File(void* a){
  char* p = (char*)commonModelDataArr;
  int i = 0;
  int n = numCommonModelData[0];
  while(i < n){
    if(a == (void*)(*(int*)p)) return p;
    i++;
    p += 0xc;
  }
  *(int*)p = (int)a;
  *(int*)(p+4) = 0;
  *(int*)(p+8) = 0;
  numCommonModelData[0] = numCommonModelData[0] + 1;
  if(data_0208e738[0] == 0) return p;
  _ZN5Model13LoadTexAndPalER8BMD_File(a);
  return p;
}
