extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned, unsigned, unsigned, int, int);
extern unsigned char DecIfAbove0_Byte(void*);
extern int func_020ca78c(void*);
extern int _ZN6Player24TryExitWhiteDoorWithStarEv(void*);

int func_ov100_02145080(char* c, void* arg1)
{
    if (*(int*)(c + 8) != 0xd) {
        if (*(unsigned char*)(c + 0x145) == 0) {
            if (func_020ca78c(arg1) == 0)
                goto ret1;
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x2b, 0, 0x7f, 0x15666, 0);
            *(unsigned char*)(c + 0x145) = 0x78;
            goto ret1;
        }
        if (DecIfAbove0_Byte((char*)c + 0x145) != 0)
            goto ret1;
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x2b, 0x7f, 0, 0x7222, 0);
        {
            unsigned char* p = (unsigned char*)(((int)c + 0x145) & 0xFFFFFFFFFFFFFFFF);
            *p += 1;
        }
        return 0;
    }
    if (*(unsigned char*)(c + 0x145) != 0)
        goto ret0;
    *(unsigned char*)(c + 0x145) = _ZN6Player24TryExitWhiteDoorWithStarEv(arg1);
    goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}
