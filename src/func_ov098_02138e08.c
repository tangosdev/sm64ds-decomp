extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void*, void*, unsigned int, int, short);
void func_ov098_02138e08(char* c){
  int v[3];
  if(*(unsigned char*)(c+0x607) == 1) return;
  v[0] = *(int*)(c+0x5c);
  v[1] = *(int*)(c+0x60);
  v[2] = *(int*)(c+0x64);
  _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v, 3, 0xf000, 0);
  *(unsigned char*)(c+0x607) = 1;
}
