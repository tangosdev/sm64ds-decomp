//cpp
extern "C" {
extern unsigned char data_0209caa0[];
}

struct SaveData {
    int NumGlowingRabbitsFound(void);
};

int SaveData::NumGlowingRabbitsFound(void)
{
int count = 0;
  int f = *(int*)(data_0209caa0+8);
  unsigned int mask = 0x100000;
  int i = 0;
  do {
    i++;
    if(f & mask) count++;
    mask <<= 1;
  } while(i < 8);
  return count;

}
