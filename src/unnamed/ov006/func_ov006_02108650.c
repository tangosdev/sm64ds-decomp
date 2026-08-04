extern int data_ov006_0212ed4c[];
extern int data_ov006_0212ed68[];

int func_ov006_02108650(int x, int y)
{
    int r = 0x25;

    if (x > data_ov006_0212ed4c[0] && x <= data_ov006_0212ed4c[1]) {
        if (y > 0x10 && y <= 0x60) {
            r = 0x23;
        } else if (y > 0x60 && y <= 0xb0) {
            r = 0x24;
        }
    } else if (x > data_ov006_0212ed4c[1] && x <= data_ov006_0212ed4c[2]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 0;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x14;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 3;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x17;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 6;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x1a;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 9;
        }
    } else if (x > data_ov006_0212ed4c[2] && x <= data_ov006_0212ed4c[3]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 0xc;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x1d;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 0xe;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x1f;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 0x10;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x21;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0x12;
        }
    } else if (x > data_ov006_0212ed4c[3] && x <= data_ov006_0212ed4c[4]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 1;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x15;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 4;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x18;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 7;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x1b;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0xa;
        }
    } else if (x > data_ov006_0212ed4c[4] && x <= data_ov006_0212ed4c[5]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 0xd;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x1e;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 0xf;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x20;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 0x11;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x22;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0x13;
        }
    } else if (x > data_ov006_0212ed4c[5] && x <= data_ov006_0212ed4c[6]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 2;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x16;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 5;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x19;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 8;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x1c;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0xb;
        }
    }
    return r;
}
