typedef int Fix12i;
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist, int loop);
static const int kPoolDist = 0x8777;
int func_ov002_020f9468(char* a, unsigned short* counter){
  int ret = 0;
  if ((int)*counter < (int)*(unsigned short*)(a+0xde) - 0xf) {
    int v = *(int*)(a+0xd8);
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(*(unsigned int*)(a+0xd4), v, 0x7f, (0x7f - v)<<0xc, *(unsigned char*)(a+0xe0));
    *counter += 1;
    goto done;
  }
  if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(*(unsigned int*)(a+0xd4), 0x7f, 0, kPoolDist, *(unsigned char*)(a+0xe0)))
    return 1;
done:
  return ret;
}
