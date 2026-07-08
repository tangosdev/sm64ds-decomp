extern int LEVEL_FILE[];
int _ZN5Stage11GetSkyboxIDEv(void){
  unsigned int *p=*(unsigned int**)LEVEL_FILE;
  return (p[6] >> 4) & 0x1f;
}
