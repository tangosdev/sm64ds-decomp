/* func_ov002_020cd3e0 at 0x020cd3e0 (ov002), size 0x68
 * Compiler mwccarm 1.2/sp2p3 */
extern int _ZN6Player7SetAnimEji5Fix12IiEj(char *, int, int, int, unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void *);

void func_ov002_020cd3e0(char *c) {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa9, 0x40000000, 0x1000, 0);
    *(unsigned char *)(c + 0x6e3) = 2;
    *(short *)(c + 0x6a4) = 0xf;
    *(unsigned char *)(int)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) &= 0xfe;
    _ZN5Sound9PlayBank0EjRK7Vector3(0x14, c + 0x74);
}
