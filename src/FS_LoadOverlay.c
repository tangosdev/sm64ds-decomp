extern int func_02018c00(void *info, int proc, int id);
extern int func_02018b64(void *info);
extern void Crash(void);
extern void func_0205dd9c(char *self);
extern void func_020aa420(void);

int FS_LoadOverlay(int proc, int id) {
    char info[0x24];
    unsigned int ram;
    unsigned int sum;
    if (func_02018c00(info, proc, id)) {
        if (func_02018b64(info)) goto ok;
    }
    return 0;
ok:
    ram = *(unsigned int *)(info + 4);
    if (ram >= (unsigned int)func_020aa420) {
        sum = ram + *(unsigned int *)(info + 8);
        if (sum + *(unsigned int *)(info + 0xc) <= 0x214eaa0u)
            goto start;
    }
    Crash();
start:
    func_0205dd9c(info);
    return 1;
}
