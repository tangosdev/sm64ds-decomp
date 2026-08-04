//cpp
namespace Model { int LoadTextureToVram(char *, unsigned int); }
extern "C" {
extern unsigned int func_02045a50(const void *src, unsigned int size);
extern char data_ov006_0213b414[];
extern void *data_ov006_02140844;
extern char data_ov006_0213b3f4[];
extern void *data_ov006_02140814;
void func_ov006_020d0b2c(void) {
  data_ov006_02140844 = (void *)Model::LoadTextureToVram(data_ov006_0213b414, 0x400);
  data_ov006_02140814 = (void *)func_02045a50(data_ov006_0213b3f4, 0x20);
}
}
