extern unsigned short data_02092664;
extern unsigned int data_0209f5b8;
int _ZN5Scene15SetSceneToSpawnEjj(unsigned int a, unsigned int b){
  if (a != data_02092664){
    data_02092664 = a;
    data_0209f5b8 = b;
    return 1;
  }
  return 0;
}
