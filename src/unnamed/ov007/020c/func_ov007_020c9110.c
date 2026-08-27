typedef struct {
    int f0;
    int f4;
    int f8;
    short f_c;
    short _e;
    short f10;
    unsigned short f12;
    int f14;
    int f18;
} Obj;

void func_ov007_020c9110(Obj *this) {
    if (this->f_c == -1) return;
    {
        int f8 = this->f8;
        if ((f8 & 1) == 0) {
            int thresh = (this->f12 - 1) << 12;
            int flag = 0;
            switch (this->f0) {
            case 0: {
                int *p = (int *)(((long long)(int)&this->f14) & -1LL);
                *p += this->f18;
                if (this->f14 > thresh) flag = 2;
                else if (this->f14 < 0) flag = 1;
                break;
            }
            case 1: break;
            case 2: break;
            }
            if (flag != 0) {
                switch (this->f4) {
                case 0:
                    this->f0 = 2;
                    break;
                case 1:
                    if (flag == 1) flag = 2; else flag = 1;
                    break;
                case 2: {
                    int *p = (int *)(((long long)(int)&this->f18) & -1LL);
                    *p *= -1;
                    break;
                }
                }
                if (flag == 1) thresh = 0;
                this->f14 = thresh;
            }
        } else {
            this->f8 = f8 & ~1;
        }
    }
    this->f10 = this->f14 >> 12;
}
