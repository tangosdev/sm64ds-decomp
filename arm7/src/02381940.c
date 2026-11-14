extern int SDK_IRQ_STACKSIZE;
extern int SDK_SYS_STACKSIZE;

int fn_02381940(int a1) {
    int irqStackLo;
    int size;
    switch (a1) {
    case 1:
        return 0x027FF000;
    case 7:
        return 0x03800000;
    case 8:
        irqStackLo = 0x0380FF80 - (int)&SDK_IRQ_STACKSIZE;
        size = (int)&SDK_SYS_STACKSIZE;
        if (size == 0)
            return 0x03800000;
        if (size < 0)
            return 0x03800000 - size;
        return irqStackLo - size;
    default:
        return 0;
    }
}
