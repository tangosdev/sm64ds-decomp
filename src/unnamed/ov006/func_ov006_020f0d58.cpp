//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov006_02142204[];

extern "C" int func_ov006_020f0d58(C *c)
{
    int count = 0;
    int i = 0;
    char *p = (char *)c;
    do {
        if (*(unsigned char *)(p + 0x47b4) != 0) {
            (c->*data_ov006_02142204[*(unsigned char *)(p + 0x47b6)].pmf)(i);
            count++;
        }
        i++;
        p += 0x18;
    } while (i < 2);
    return count == 0;
}
