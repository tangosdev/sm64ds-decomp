//cpp
#include "types.h"
#include "private/ov006_m8c.h"

extern "C" {

extern unsigned char *data_ov006_0213e070[];
extern unsigned char *data_ov006_0213ded0[];

void func_ov006_021053a8(Ov006M8c *o)
{
    char *c = (char *)o;
    int n;
    int found;
    int i;
    int j;
    int li;
    int nlights;
    int three;
    int two;
    int z0;
    int z1;
    int z2;
    int arr[36];
    int x0;
    int w;
    int idx;
    int wq;
    int hq;
    int col;
    int row;
    int y;
    int x;
    int cell;
    unsigned char *p;

    if (((unsigned short *)(c + 0x4e00))[0x62] != 0) {
        (*(unsigned short *)(c + 0x4ec4))--;
        if (((short *)(c + 0x4e00))[0x62] < 0) {
            ((short *)(c + 0x4e00))[0x62] = 0;
        }
        return;
    }

    n = *(int *)(c + 0x4cb8);
    found = 0;
    i = found;
    if (n > 0) {
        do {
            if (*(unsigned char *)(c + i + 0x4efa) != 0) {
                found++;
                break;
            }
            i++;
        } while (i < n);
    }
    if (found) {
        return;
    }

    i = 0;
    do {
        arr[i] = 0;
        i++;
    } while (i < 0x24);
    j = 0;

    if (n > 0) {
        do {
            idx = *(int *)(c + 0x4cb4);
            if (o->m8c() != 0) {
                arr[j] = data_ov006_0213ded0[idx][j];
            } else {
                arr[j] = data_ov006_0213e070[idx][j];
            }
            n = *(int *)(c + 0x4cb8);
            j++;
        } while (j < n);
    }

    nlights = *(unsigned char *)(c + 0x4fde);
    li = 0;
    if (nlights > 0) {
        x0 = li;
        w = o->width;
        z0 = x0;
        z1 = x0;
        z2 = x0;
        two = 2;
        three = 3;
        do {
            idx = *(unsigned char *)(c + li + 0x4fae);
            wq = three;
            hq = three;
            col = idx % o->width;
            row = idx / o->width;
            if (col == 0 || col == o->width - 1) {
                wq = two;
            }
            if (row == 0 || row == o->width - 1) {
                hq = two;
            }
            if (--col < 0) {
                col = z0;
            }
            if (--row < 0) {
                row = z1;
            }
            y = z2;
            if (hq > 0) {
                do {
                    x = x0;
                    if (wq > 0) {
                        do {
                            cell = o->width * (row + y) + (col + x);
                            arr[cell] ^= 1;
                            x++;
                        } while (x < wq);
                    }
                    y++;
                } while (y < hq);
            }
            li++;
        } while (li < nlights);
    }

    j = 0;
    if (n > 0) {
        do {
            p = (unsigned char *)(c + j + 0x4f1e);
            if (*p != arr[j]) {
                *(unsigned char *)(c + j + 0x4efa) = 1;
                *p ^= 1;
                *(unsigned char *)(c + j + 0x4f8a) = 1;
            }
            j++;
        } while (j < *(int *)(c + 0x4cb8));
    }

    ((unsigned short *)(c + 0x4e00))[0x62] = 0x40;
    *(unsigned char *)(c + 0x4fe0) = 3;
    *(unsigned char *)(c + 0x4fe5) = 0;
}

}
