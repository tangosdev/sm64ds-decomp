extern void func_ov004_020ad90c(void);
extern void func_ov004_020b0aa0(int arg);
extern void func_0203cbc0(void);
extern void func_02012e1c(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int x);
extern void func_ov004_020b2c84(void);
extern void _ZN5Scene21AfterCleanupResourcesEj(char* c, unsigned int x);
extern int data_0209b308[];
extern int SCENE_RELATED[];
extern int data_ov004_020beb74[];
extern int data_ov004_020beb60[];

void func_ov004_020b0840(char* c, int arg){
  if (arg == 2) {
    if (data_0209b308[4] == 0)
      func_ov004_020ad90c();
    SCENE_RELATED[0] = 0;
    data_ov004_020beb74[1] = 0;
    func_ov004_020b0aa0(0x1d);
    if (data_ov004_020beb60[0] != 0) {
      func_0203cbc0();
      data_ov004_020beb60[0] = 0;
    }
    if (*(int*)(c + 0x4628) != 0) {
      func_02012e1c();
      _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
    }
    func_ov004_020b2c84();
  }
  _ZN5Scene21AfterCleanupResourcesEj(c, arg);
}
