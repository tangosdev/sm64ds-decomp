extern signed char byte_380583C[];

int fn_0238385C(int index) {
    int value;
    if (index < 0x20)
        return byte_380583C[index];
    if (index < 0x40)
        return byte_380583C[0x40 - index];
    if (index < 0x60) {
        value = -byte_380583C[index - 0x40];
        return (signed char)value;
    }
    index -= 0x60;
    value = -byte_380583C[0x20 - index];
    return (signed char)value;
}
