// Snaps this object to the actor it follows (+0x38c): copies its position
// (three words at +0x5c) and Y angle (+0x8e), mirrors the position into
// +0x3d8, and copies its rotation triple (+0x8c) into both +0x3e4 and +0x3ea.
void func_ov002_020ed5b0(char* c)
{
    char* src;
    int* sp;
    short* m;
    src = *(char**)(c + 0x38c);
    sp = (int*)((long long)(int)(src + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
    *(int*)(c + 0x5c) = sp[0];
    *(int*)(c + 0x60) = sp[1];
    *(int*)(c + 0x64) = sp[2];
    src = *(char**)(c + 0x38c);
    *(short*)(c + 0x8e) = *(short*)(src + 0x8e);
    *(int*)(c + 0x3d8) = *(int*)(c + 0x5c);
    *(int*)(c + 0x3dc) = *(int*)(c + 0x60);
    *(int*)(c + 0x3e0) = *(int*)(c + 0x64);
    src = *(char**)(c + 0x38c);
    m = (short*)((long long)(int)(src + 0x8c) & 0xFFFFFFFFFFFFFFFFLL);
    *(short*)(c + 0x3e4) = m[0];
    *(short*)(c + 0x3e6) = m[1];
    *(short*)(c + 0x3e8) = m[2];
    src = *(char**)(c + 0x38c);
    m = (short*)((unsigned long long)(unsigned int)(src + 0x8c));
    *(short*)(c + 0x3ea) = m[0];
    *(short*)(c + 0x3ec) = m[1];
    *(short*)(c + 0x3ee) = m[2];
}
