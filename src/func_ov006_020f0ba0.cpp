//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_0214221c[];
struct Row { unsigned char tag; char pad[0x17]; };
struct C { char pad[0x47b7]; Row rows[1]; };
extern "C" void func_ov006_020f0ba0(C* c, int i){
  (c->*(data_ov006_0214221c[c->rows[i].tag].pmf))(i);
}
