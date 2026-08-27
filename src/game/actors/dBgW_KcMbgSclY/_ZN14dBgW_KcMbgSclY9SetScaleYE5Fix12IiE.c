typedef int Fix12i;

typedef struct dBgW_KcMbgSclY {
    char _pad[0x1c8];
    Fix12i scaleY;
    Fix12i invScaleY;
} dBgW_KcMbgSclY;

extern Fix12i func_02053200(Fix12i x);

void _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(dBgW_KcMbgSclY* this, Fix12i newScaleY) {
    this->scaleY = newScaleY;
    if (this->scaleY == 0) return;
    this->invScaleY = func_02053200(this->scaleY);
}