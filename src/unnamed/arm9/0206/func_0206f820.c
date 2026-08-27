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

enum { only_minus = 0, sign_always = 1, space_holder = 2 };
enum { left_justification = 0, right_justification = 1, zero_fill = 2 };

char *func_0206f820(long long num, char *buff_end, print_format format)
{
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
