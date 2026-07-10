int _ZN9CameraTag13InitResourcesEv(char* c){
  *(unsigned int*)(c+0x80)=(((*(unsigned int*)(c+8)&0xf)+1)*0x64000)>>1;
  *(unsigned int*)(c+0x84)=(((*(unsigned int*)(c+8)>>4&0xf)+1)*0x64000);
  *(short*)(c+0x8e)=-*(short*)(c+0x8e);
  return 1;
}
