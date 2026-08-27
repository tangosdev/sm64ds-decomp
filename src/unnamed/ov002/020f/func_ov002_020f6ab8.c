struct Vector3;
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern int _ZN5Sound4PlayEjjRK7Vector3(unsigned int a, unsigned int b, struct Vector3 *v);

int func_ov002_020f6ab8(char *c, unsigned char *p)
{
    unsigned int id = ReadUnalignedInt(p);
    _ZN5Sound4PlayEjjRK7Vector3(1, id, (struct Vector3*)(c + 0x74));
    return 1;
}
