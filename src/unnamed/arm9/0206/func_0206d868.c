extern void _ZN4CP1514MPUDataRegion7Ej(unsigned int val);
extern void _ZN3IRQ11SetFIQStateEb(int state);
extern void _ZN3IRQ11SetIRQStateEb(int state);

typedef struct {
    int irqState;
    int fiqState;
    unsigned int mpuDataRegion7;
} SavedSystemState;

void func_0206d868(const SavedSystemState* s)
{
    _ZN4CP1514MPUDataRegion7Ej(s->mpuDataRegion7);
    _ZN3IRQ11SetFIQStateEb(s->fiqState);
    _ZN3IRQ11SetIRQStateEb(s->irqState);
}
