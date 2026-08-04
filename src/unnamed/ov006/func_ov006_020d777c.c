#pragma opt_common_subs off
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

void func_02012718(void *a, int b);

void func_ov006_020d777c(char *this, int idx)
{
    int n = idx << 6;
    if (*(u16*)(this + (idx << 6) + 0x4600 + 0x90) != 0) {
        *(u16*)(this + 0x4690 + n) -= 1;
        if (*(s16*)(this + (idx << 6) + 0x4600 + 0x90) < 0)
            *(u16*)(this + (idx << 6) + 0x4600 + 0x90) = 0;
        return;
    }
    {
        int rp = *(u8*)(this + (idx << 6) + 0x4000 + 0x69c);
        int quo = 0;
        int curX, curY, targetY, targetX;
        while (rp >= 10) { rp -= 10; quo++; }
        n = quo * 20;
        targetX = rp * 16 + 0x38;
        targetY = n - 0xC0;
        curX = *(int*)(this + (idx << 6) + 0x4000 + 0x660) >> 12;
        curY = *(int*)(this + (idx << 6) + 0x4000 + 0x664) >> 12;
        if (targetX == curX && targetY == curY) {
            *(u8*)(this + (idx << 6) + 0x4000 + 0x69b) = 4;
            if (*(u8*)(this + (idx << 6) + 0x4000 + 0x696) != 0)
                func_02012718((void*)0x1d9, *(int*)(this + (idx << 6) + 0x4000 + 0x660));
            else
                func_02012718((void*)0x1da, *(int*)(this + (idx << 6) + 0x4000 + 0x660));
            return;
        }
        if (targetY != curY) {
            *(int*)(this + 0x4664 + (idx << 6)) += *(int*)(this + (idx << 6) + 0x4000 + 0x670);
            if ((*(int*)(this + (idx << 6) + 0x4000 + 0x664) >> 12) >= targetY)
                *(int*)(this + (idx << 6) + 0x4000 + 0x664) = targetY << 12;
            return;
        }
        if (targetX > curX) {
            *(int*)(this + 0x4660 + (idx << 6)) += *(int*)(this + (idx << 6) + 0x4000 + 0x670);
            if (targetX <= (*(int*)(this + (idx << 6) + 0x4000 + 0x660) >> 12))
                *(int*)(this + (idx << 6) + 0x4000 + 0x660) = targetX << 12;
        } else if (targetX < curX) {
            *(int*)(this + 0x4660 + (idx << 6)) -= *(int*)(this + (idx << 6) + 0x4000 + 0x670);
            if (targetX >= curX)
                *(int*)(this + (idx << 6) + 0x4000 + 0x660) = targetX << 12;
        }
    }
}
