struct S {
    unsigned char b0;
    unsigned char pad1;
    short s2;
    unsigned char b4;
    unsigned char data[1];
};

extern void func_02071644(struct S *out, int len);

void func_02071364(struct S *out, struct S *a, struct S *b)
{
    unsigned char buf[68];
    unsigned int carry;
    int sb;
    unsigned char *p;
    unsigned char *end;
    int i;

    carry = 0;
    sb = a->b4 + b->b4 - 1;
    p = buf + sb + 1;
    end = p;
    out->b0 = 0;
    if (sb > 0) {
        do {
            int count;
            unsigned char *pa;
            unsigned char *pb;
            int startA;
            int startB;
            int rem;

            startB = b->b4 - 1;
            startA = sb - startB - 1;
            if (startA < 0) {
                startA = 0;
                startB = sb - 1;
            }
            pa = &a->data[startA];
            rem = a->b4 - startA;
            pb = &b->data[startB];
            count = startB + 1;
            if (count > rem) count = rem;
            while (count > 0) {
                carry += (*pa) * (*pb);
                count--;
                pa++;
                pb--;
            }
            *--p = (unsigned char)(carry % 10);
            carry = carry / 10;
            sb--;
        } while (sb > 0);
    }

    {
        out->s2 = a->s2 + b->s2;
        if (carry != 0) {
            /* Launder so ep materializes as addne r2,r0,#2 (not folded [r0,#2]). */
            short *ep = (short *)((long long)(int)((char *)out + 2) & 0xFFFFFFFFFFFFFFFFLL);
            *--p = (unsigned char)carry;
            *ep = (short)(*ep + 1);
        }
    }

    i = 0;
    while (i < 0x20 && p < end) {
        unsigned char v = *p;
        out->data[i] = v;
        i++;
        p++;
    }
    out->b4 = (unsigned char)i;
    if (p >= end) return;

    {
        unsigned char d;
        d = *p;
        if (d < 5) return;
        if (d != 5) goto docall;
        {
            unsigned char *q;
            q = p + 1;
            while (q < end) {
                if (*q != 0) goto docall;
                q++;
            }
        }
        if ((*(p - 1) & 1) == 0) return;
    }
docall:
    func_02071644(out, out->b4);
}
