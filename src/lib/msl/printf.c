/* MSL printf.c's conversion helpers. The cartridge keeps only this
 * contiguous helper run from the file; __pformatter and the output wrappers
 * immediately precede it in the legacy cstd4 reconstruction. Source order is
 * the reverse of the ROM because mwcc emits functions in reverse order. */
#include "Decimal.h"
#include "msl/string.h"

typedef struct {
    unsigned char justification_options;
    unsigned char sign_options;
    unsigned char precision_specified;
    unsigned char alternate_form;
    unsigned char argument_options;
    unsigned char conversion_char;
    char fill_char;
    char pad7;
    int field_width;
    int precision;
} print_format;

typedef struct {
    unsigned char sgn;
    unsigned char unused;
    short exp;
    struct {
        unsigned char length;
        unsigned char text[36];
    } sig;
} decimal;

enum { only_minus = 0, sign_always = 1, space_holder = 2 };
enum { left_justification = 0, right_justification = 1, zero_fill = 2 };
enum {
    normal_argument = 0,
    char_argument,
    short_argument,
    long_argument,
    intmax_argument,
    size_t_argument,
    ptrdiff_argument,
    long_long_argument,
    long_double_argument,
    wchar_argument
};

extern void __num2dec(void *f, double x, void *d);
extern int func_01ff9e2c(double a, double b);
extern double fabs(double x);
extern const unsigned short data_02086a58[132];
extern char data_0209a5ec[];
extern char data_0209a5f4[];
extern char data_0209a5fc[];
extern char data_0209a600[];
extern char data_0209a604[];
extern char data_0209a60c[];
extern char data_0209a614[];
extern char data_0209a618[];
extern char data_0209a61c[];

#define ISUPPER(c) (((c) < 0 || (c) >= 128) ? 0 : (data_02086a58[c] & 0x200))
#define isdigit_(c) ((c) < 0 || (c) >= 128 ? 0 : data_02086a58[c] & 8)

unsigned char *parse_format(unsigned char *format_string, char **arg, print_format *format);
char *long2str(long num, char *buff_end, print_format format);
char *longlong2str(long long num, char *buff_end, print_format format);
char *double2hex(double x, char *buff_end, print_format format);
void round_decimal(Decimal *dec, int new_length);
char *float2str(double x, char *buff_end, print_format format);

/* Parse one printf conversion specification into a print_format. */
unsigned char *parse_format(unsigned char *format_string, char **arg, print_format *format) {
    print_format f;
    unsigned char *s = format_string;
    int c;
    int flag_found;

    f.justification_options = right_justification;
    f.sign_options = only_minus;
    f.precision_specified = 0;
    f.alternate_form = 0;
    f.argument_options = normal_argument;
    f.field_width = 0;
    f.precision = 0;

    s = (format_string + 1);
    if ((c = *s) == '%') {
        f.conversion_char = c;
        *format = f;
        return s + 1;
    }

    do {
        flag_found = 1;

        switch (c) {
        case '-':
            f.justification_options = left_justification;
            break;
        case '+':
            f.sign_options = sign_always;
            break;
        case ' ':
            if (f.sign_options != sign_always)
                f.sign_options = space_holder;
            break;
        case '#':
            f.alternate_form = 1;
            break;
        case '0':
            if (f.justification_options != left_justification)
                f.justification_options = zero_fill;
            break;
        default:
            flag_found = 0;
            break;
        }

    } while (flag_found && (c = *++s, 1));

    if (c == '*') {
        *arg += 4;
        {
            int *slot = (int *)(*arg - 4);
            if ((f.field_width = *slot) < 0) {
                f.justification_options = left_justification;
                f.field_width = -f.field_width;
            }
        }
        c = *++s;
    } else {
        while (isdigit_(c)) {
            f.field_width = (f.field_width * 10) + (c - '0');
            c = *++s;
        }
    }

    if (f.field_width > 509) {
        f.conversion_char = 0xFF;
        *format = f;
        return s + 1;
    }

    if (c == '.') {
        f.precision_specified = 1;

        c = *++s;

        if (c == '*') {
            *arg += 4;
            {
                int *slot = (int *)(*arg - 4);
                if ((f.precision = *slot) < 0)
                    f.precision_specified = 0;
            }
            c = *++s;
        } else {
            while (isdigit_(c)) {
                f.precision = (f.precision * 10) + (c - '0');
                c = *++s;
            }
        }
    }

    flag_found = 1;

    switch (c) {
    case 'h':
        f.argument_options = short_argument;
        if (*(s + 1) == 'h') {
            f.argument_options = char_argument;
            c = *++s;
        }
        break;
    case 'l':
        f.argument_options = long_argument;
        if (*(s + 1) == 'l') {
            f.argument_options = long_long_argument;
            c = *++s;
        }
        break;
    case 'L':
        f.argument_options = long_double_argument;
        break;
    case 'j':
        f.argument_options = intmax_argument;
        break;
    case 'z':
        f.argument_options = size_t_argument;
        break;
    case 't':
        f.argument_options = ptrdiff_argument;
        break;
    default:
        flag_found = 0;
        break;
    }

    if (flag_found)
        c = *++s;

    f.conversion_char = c;

    switch (c) {
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'X':
        if (f.argument_options == long_double_argument) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified)
            f.precision = 1;
        else if (f.justification_options == zero_fill)
            f.justification_options = right_justification;
        break;

    case 'f':
    case 'F':
        if (f.argument_options == short_argument ||
            (unsigned char)(f.argument_options + 0xFC) <= (long_long_argument - intmax_argument)) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified)
            f.precision = 6;
        break;

    case 'a':
    case 'A':
        if (!f.precision_specified)
            f.precision = 13;
        if (f.argument_options == short_argument || f.argument_options == intmax_argument ||
            f.argument_options == size_t_argument || f.argument_options == ptrdiff_argument ||
            f.argument_options == long_long_argument || f.argument_options == char_argument)
            f.conversion_char = 0xFF;
        break;

    case 'g':
    case 'G':
        if (f.precision == 0)
            f.precision = 1;
    case 'e':
    case 'E':
        if (f.argument_options == short_argument || f.argument_options == intmax_argument ||
            f.argument_options == size_t_argument || f.argument_options == ptrdiff_argument ||
            f.argument_options == long_long_argument || f.argument_options == char_argument)
            f.conversion_char = 0xFF;
        else if (!f.precision_specified)
            f.precision = 6;
        break;

    case 'p':
        f.conversion_char = 'x';
        f.alternate_form = 1;
        f.argument_options = long_argument;
        f.precision = 8;
        break;

    case 'c':
        if (f.argument_options == long_argument)
            f.argument_options = wchar_argument;
        else if (f.precision_specified || f.argument_options != normal_argument)
            f.conversion_char = 0xFF;
        break;

    case 's':
        if (f.argument_options == long_argument)
            f.argument_options = wchar_argument;
        else if (f.argument_options != normal_argument)
            f.conversion_char = 0xFF;
        break;

    case 'n':
        if (f.argument_options == long_double_argument)
            f.conversion_char = 0xFF;
        break;

    default:
        f.conversion_char = 0xFF;
        break;
    }

    *format = f;
    return s + 1;
}

/* Format a long integer backwards from buff_end. */
char *long2str(long num, char *buff_end, print_format format) {
    unsigned long unsigned_num, base;
    char *p;
    int n, digits;
    int minus_sign = 0;

    unsigned_num = (unsigned long)num;
    digits = minus_sign;
    p = buff_end;
    *--p = digits;

    if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
        return p;

    switch (format.conversion_char) {
    case 'd':
    case 'i':
        base = 10;
        if (num < 0) {
            minus_sign = 1;
            unsigned_num = -unsigned_num;
        }
        break;
    case 'o':
        base = 8;
        format.sign_options = only_minus;
        break;
    case 'u':
        base = 10;
        format.sign_options = only_minus;
        break;
    case 'x':
    case 'X':
        base = 16;
        format.sign_options = only_minus;
        break;
    }

    do {
        n = unsigned_num % base;
        unsigned_num /= base;
        if (n < 10)
            n += '0';
        else if (format.conversion_char == 'x')
            n += 'a' - 10;
        else
            n += 'A' - 10;
        *--p = n;
        ++digits;
    } while (unsigned_num != 0);

    if (base == 8 && format.alternate_form && *(unsigned char *)p != '0') {
        *--p = '0';
        ++digits;
    }

    if (format.justification_options == zero_fill) {
        format.precision = format.field_width;
        if (minus_sign || format.sign_options != only_minus)
            --format.precision;
        if (base == 16 && format.alternate_form)
            format.precision -= 2;
    }

    if ((buff_end - p) + format.precision > 509)
        return 0;

    while (digits < format.precision) {
        *--p = '0';
        ++digits;
    }

    if (base == 16 && format.alternate_form) {
        *--p = format.conversion_char;
        *--p = '0';
    }

    if (minus_sign)
        *--p = '-';
    else if (format.sign_options == sign_always)
        *--p = '+';
    else if (format.sign_options == space_holder)
        *--p = ' ';

    return p;
}

/* Format a long long integer backwards from buff_end. */
char *longlong2str(long long num, char *buff_end, print_format format) {
    unsigned long long base;
    char *p;
    int n, digits;
    int minus_sign = 0;

    digits = minus_sign;
    p = buff_end;
    *--p = digits;

    if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
        return p;

    switch (format.conversion_char) {
    case 'd':
    case 'i':
        base = 10;
        if (num < 0) {
            minus_sign = 1;
            num = -num;
        }
        break;
    case 'o':
        base = 8;
        format.sign_options = only_minus;
        break;
    case 'u':
        base = 10;
        format.sign_options = only_minus;
        break;
    case 'x':
    case 'X':
        base = 16;
        format.sign_options = only_minus;
        break;
    }

    do {
        n = num % base;
        num /= base;
        if (n < 10)
            n += '0';
        else if (format.conversion_char == 'x')
            n += 'a' - 10;
        else
            n += 'A' - 10;
        *--p = n;
        ++digits;
    } while (num != 0);

    if (base == 8 && format.alternate_form && (unsigned char)*p != '0') {
        *--p = '0';
        ++digits;
    }

    if (format.justification_options == zero_fill) {
        format.precision = format.field_width;
        if (minus_sign || format.sign_options != only_minus)
            --format.precision;
        if (base == 16 && format.alternate_form)
            format.precision -= 2;
    }

    if ((buff_end - p) + format.precision > 509)
        return 0;

    while (digits < format.precision) {
        *--p = '0';
        ++digits;
    }

    if (base == 16 && format.alternate_form) {
        *--p = format.conversion_char;
        *--p = '0';
    }

    if (minus_sign)
        *--p = '-';
    else if (format.sign_options == sign_always)
        *--p = '+';
    else if (format.sign_options == space_holder)
        *--p = ' ';

    return p;
}

/* Format a double in C99 hexadecimal floating-point notation. */
char *double2hex(double x, char *buff_end, print_format format) {
    decform form;
    print_format exp_format;
    decimal dec;
    char *p;
    unsigned char digit;
    unsigned long exponent;
    int prec;
    int sign_opt;
    int alt;
    int code;
    int i;
    unsigned char *q;
    unsigned char *r;
    int n;
    int bitpos;

    prec = format.precision;
    code = format.conversion_char;
    alt = format.alternate_form;
    sign_opt = format.sign_options;

    if (prec > 509)
        return 0;

    form.style = 0;
    form.digits = 32;

    __num2dec(&form, x, &dec);

    if (fabs(x) == 0.0) {
        p = buff_end - 6;
        strcpy(p, data_0209a61c);
        return p;
    }

    if (dec.sig.text[0] == 'I') {
        if (dec.sgn) {
            p = buff_end - 5;
            if (code == 'A')
                strcpy(p, data_0209a5ec);
            else
                strcpy(p, data_0209a5f4);
        } else {
            p = buff_end - 4;
            if (code == 'A')
                strcpy(p, data_0209a5fc);
            else
                strcpy(p, data_0209a600);
        }
        return p;
    } else if (dec.sig.text[0] == 'N') {
        if (dec.sgn) {
            p = buff_end - 5;
            if (code == 'A')
                strcpy(p, data_0209a604);
            else
                strcpy(p, data_0209a60c);
        } else {
            p = buff_end - 4;
            if (code == 'A')
                strcpy(p, data_0209a614);
            else
                strcpy(p, data_0209a618);
        }
        return p;
    }

    exp_format.justification_options = right_justification;
    exp_format.sign_options = sign_always;
    exp_format.precision_specified = 0;
    exp_format.alternate_form = 0;
    exp_format.argument_options = 0;
    exp_format.field_width = 0;
    exp_format.precision = 1;
    exp_format.conversion_char = 'd';

    q = (unsigned char *)&x;

    for (i = 0; i < 4; i++) {
        digit = q[i];
        q[i] = q[7 - i];
        q[7 - i] = digit;
    }

    exponent = (((unsigned char *)&x)[0] << 25 | ((unsigned char *)&x)[1] << 17);
    exponent = (exponent >> 21) & 0x7ff;

    p = long2str(exponent - 1023, buff_end, exp_format);

    if (code == 'a')
        *--p = 'p';
    else
        *--p = 'P';

    bitpos = prec * 4 + 11;
    r = (unsigned char *)&x;

    for (n = prec; n >= 1; n--) {
        if (bitpos < 64) {
            digit = r[bitpos >> 3] >> (7 - (bitpos & 7));

            if ((bitpos & ~7) != ((bitpos - 4) & ~7))
                digit |= (r[(bitpos >> 3) - 1] << 8) >> (7 - (bitpos & 7));

            digit &= 15;

            if (digit < 10)
                digit += '0';
            else if (code == 'a')
                digit += 'a' - 10;
            else
                digit += 'A' - 10;
        } else {
            digit = '0';
        }

        *--p = digit;
        bitpos -= 4;
    }

    if (prec || alt)
        *--p = '.';

    *--p = '1';

    if (code == 'a')
        *--p = 'x';
    else
        *--p = 'X';

    *--p = '0';

    if (dec.sgn)
        *--p = '-';
    else if (sign_opt == sign_always)
        *--p = '+';
    else if (sign_opt == space_holder)
        *--p = ' ';

    return p;
}

/* Round a decimal to new_length digits, half to even. */
void round_decimal(Decimal *dec, int new_length) {
    unsigned char *digits;
    unsigned char *q;
    unsigned char *end;
    unsigned int d;
    unsigned int e;
    int carry;
    int len;

    if (new_length < 0) {
    reset:
        dec->exp = 0;
        dec->sig.length = 1;
        dec->sig.text[0] = 0x30;
        return;
    }
    len = dec->sig.length;
    if (new_length >= len)
        return;

    digits = &dec->sig.text[0];
    q = digits + new_length + 1;
    d = (*--q - 0x30) & 0xff;
    if (d == 5) {
        end = digits + len;
        do {
            end--;
        } while (end > q && *end == 0x30);
        if (end == q)
            carry = q[-1] & 1;
        else
            carry = 1;
    } else {
        carry = (d > 5);
    }

    if (new_length != 0) {
        do {
            e = ((*--q - 0x30) + carry) & 0xff;
            carry = (e > 9);
            if (carry != 0 || e == 0) {
                new_length--;
            } else {
                *q = e + 0x30;
                break;
            }
        } while (new_length != 0);
    }

    if (carry != 0) {
        short *pc = (short *)((int)dec + 2);
        *pc = *pc + 1;
        dec->sig.length = 1;
        dec->sig.text[0] = 0x31;
        return;
    }
    if (new_length == 0)
        goto reset;
    dec->sig.length = new_length;
}

/* Format a double backwards from buff_end for %e/%f/%g. */
char *float2str(double x, char *buff_end, print_format format) {
    int int_digits;
    int code;
    decform form;
    int prec;
    int sign_opt;
    unsigned char *q;
    int power;
    int n;
    char sign;
    int alt;
    int digits;
    Decimal dec;
    unsigned char *p;

    form.style = 0;
    form.digits = 32;

    code = format.conversion_char;
    prec = (int)format.precision;
    alt = format.alternate_form;
    sign_opt = format.sign_options;

    __num2dec(&form, x, &dec);

    p = dec.sig.text + dec.sig.length; /* strip off trailing zeroes */

    while (dec.sig.length > 1 && *--p == '0') {
        --dec.sig.length;
        ++dec.exp;
    }

    switch (*dec.sig.text) {
    case '0':
        dec.exp = 0; /* kill spurious exponent */
        break;

    case 'I':
        if (func_01ff9e2c(x, 0.0)) {
            p = (unsigned char *)buff_end - 5;
            if (ISUPPER(code))
                strcpy((char *)p, data_0209a5ec);
            else
                strcpy((char *)p, data_0209a5f4);
        } else {
            p = (unsigned char *)buff_end - 4;
            if (ISUPPER(code))
                strcpy((char *)p, data_0209a5fc);
            else
                strcpy((char *)p, data_0209a600);
        }
        return (char *)p;

    case 'N':
        if (dec.sign) {
            p = (unsigned char *)buff_end - 5;
            if (ISUPPER(code))
                strcpy((char *)p, data_0209a604);
            else
                strcpy((char *)p, data_0209a60c);
        } else {
            p = (unsigned char *)buff_end - 4;
            if (ISUPPER(code))
                strcpy((char *)p, data_0209a614);
            else
                strcpy((char *)p, data_0209a618);
        }
        return (char *)p;
    }

    dec.exp += dec.sig.length - 1; /* shift decimal point to right of sig */

    p = (unsigned char *)buff_end;
    *--p = 0;

    switch (code) {
    case 'g':
    case 'G':
        if (dec.sig.length > prec)
            round_decimal(&dec, prec);

        if (dec.exp < -4 || dec.exp >= prec) {
            if (alt)
                --prec;
            else
                prec = dec.sig.length - 1;

            if (code == 'g')
                code = 'e';
            else
                code = 'E';

            goto e_format;
        }

        if (alt)
            prec -= dec.exp + 1;
        else if ((prec = dec.sig.length - (dec.exp + 1)) < 0)
            prec = 0;

        goto f_format;

    case 'e':
    case 'E':
    e_format:
        if (dec.sig.length > prec + 1)
            round_decimal(&dec, prec + 1);

        power = dec.exp;
        sign = '+';

        if (power < 0) {
            power = -power;
            sign = '-';
        }

        for (n = 0; power || n < 2;) {
            *--p = power % 10 + '0';
            power /= 10;
            ++n;
        }

        *--p = sign;
        *--p = code;

        if (prec + (buff_end - (char *)p) > 509)
            return 0;

        if (dec.sig.length < prec + 1) {
            n = prec + 2 - dec.sig.length;

            while (--n)
                *--p = '0';
        }

        q = dec.sig.text + dec.sig.length;
        n = dec.sig.length;

        while (--n)
            *--p = *--q;

        if (prec || alt)
            *--p = '.';

        *--p = *dec.sig.text;

        if (dec.sign)
            *--p = '-';
        else if (sign_opt == sign_always)
            *--p = '+';
        else if (sign_opt == space_holder)
            *--p = ' ';
        break;

    case 'f':
    case 'F':
    f_format:
        if ((digits = dec.sig.length - dec.exp - 1) < 0)
            digits = 0;

        if (digits > prec) {
            round_decimal(&dec, dec.sig.length - (digits - prec));

            if ((digits = dec.sig.length - dec.exp - 1) < 0)
                digits = 0;
        }

        if ((int_digits = dec.exp + 1) < 0)
            int_digits = 0;

        if (int_digits + digits > 509)
            return 0;

        q = dec.sig.text + dec.sig.length;

        for (n = 0; n < prec - digits; ++n) /* trailing fractional zeroes */
            *--p = '0';

        {
            int i;
            for (i = 0; i < digits && i < dec.sig.length; ++i) /* fractional digits */
                *--p = *--q;

            for (; i < digits; ++i) /* fractional zeroes */
                *--p = '0';
        }

        if (prec || alt)
            *--p = '.';

        if (int_digits) {
            int i;
            for (i = 0; i < int_digits - dec.sig.length; ++i) /* integral zeroes */
                *--p = '0';

            for (; i < int_digits; ++i) /* integral digits */
                *--p = *--q;
        } else
            *--p = '0';

        if (dec.sign)
            *--p = '-';
        else if (sign_opt == sign_always)
            *--p = '+';
        else if (sign_opt == space_holder)
            *--p = ' ';
        break;
    }

    return (char *)p;
}

const unsigned short data_02086a58[132] = {
    0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0106, 0x0104, 0x0104,
    0x0104, 0x0104, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
    0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0142, 0x00d0, 0x00d0, 0x00d0,
    0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0,
    0x0458, 0x0458, 0x0458, 0x0458, 0x0458, 0x0458, 0x0458, 0x0458, 0x0458, 0x0458, 0x00d0, 0x00d0,
    0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x0651, 0x0651, 0x0651, 0x0651, 0x0651, 0x0651, 0x0251,
    0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251,
    0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x0251, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x00d0,
    0x00d0, 0x0471, 0x0471, 0x0471, 0x0471, 0x0471, 0x0471, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071,
    0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071, 0x0071,
    0x0071, 0x0071, 0x0071, 0x00d0, 0x00d0, 0x00d0, 0x00d0, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000,
};
