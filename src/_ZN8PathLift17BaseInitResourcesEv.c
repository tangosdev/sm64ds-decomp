
typedef unsigned int u32;
typedef unsigned short u16;
void *_ZN9ActorBasenwEj(u32 sz);
void func_ov004_020b2adc(void *p);
void func_020733a8(void *o, int a, int b, void *f1, void *f2);
void _ZN8Particle10SysTrackerC1Ev(void *p);
void _ZN5ModelC1Ev(void *p);
extern char data_ov006_0213c62c[];
extern char _ZTV17MgBounceAndPounce[];
extern char data_ov006_0213ccfc[];
void func_ov006_020c893c();
void func_ov006_020c8a04();
void func_ov006_020c6f70();
void func_ov006_020eed64();
void func_ov006_020efc08();
void _ZN6Player18St_LevelEnter_MainEv();
void *_ZN8PathLift17BaseInitResourcesEv()
{
  char *p = (char *) _ZN9ActorBasenwEj(0x5a78);
  if (p)
  {
    char *e;
    char *end;
    func_ov004_020b2adc(p);
    *((char **) p) = _ZTV17MgBounceAndPounce;
    *((u16 *) (p + 0x4664)) = 0;
    e = p + 0x466c;
    end = p;
    end = end + 0x47e4;
    do
    {
      e += 0xbc;
    }
    while (e != end);
    _ZN8Particle10SysTrackerC1Ev(p + 0x47e4);
    *((char **) p) = data_ov006_0213ccfc;
    func_020733a8(p + 0x500c, 3, 0xb8, (void *) func_ov006_020c8a04, (void *) func_ov006_020c893c);
    func_020733a8(p + 0x5234, 6, 0xf0, (void *) func_ov006_020c6f70, (void *) _ZN6Player18St_LevelEnter_MainEv);
    func_020733a8(p + 0x57d4, 0x10, 0x24, (void *) func_ov006_020efc08, (void *) func_ov006_020eed64);
    _ZN5ModelC1Ev(p + 0x5a14);
  }
  return p;
}
