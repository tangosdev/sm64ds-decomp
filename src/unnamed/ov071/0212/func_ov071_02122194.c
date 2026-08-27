extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *pos);

void func_ov071_02122194(char *self)
{
    *(unsigned short *)(self + 0x326) = 0;
    *(unsigned short *)(self + 0x324) = 0x3c;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x58, self + 0x74);
}
