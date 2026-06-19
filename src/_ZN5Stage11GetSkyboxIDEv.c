extern int data_0209f340[];
int _ZN5Stage11GetSkyboxIDEv(void){
  unsigned int *p=*(unsigned int**)data_0209f340;
  return (p[6] >> 4) & 0x1f;
}
