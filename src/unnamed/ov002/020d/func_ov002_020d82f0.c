int func_ov002_020d82f0(void* c) {
  if (*(unsigned short*)((char*)c+0x6a0) != 0) return 0;
  if (*(unsigned char*)((char*)c+0x703) != 0) return 0;
  if (*(unsigned short*)((char*)c+0x73c) != 0) return 0;
  if (*(unsigned char*)((char*)c+0x6fb) != 0) return 0;
  if (*(unsigned char*)((char*)c+0x708) != 0) goto ret0;
  if (*(unsigned char*)((char*)c+0x709) == 0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
