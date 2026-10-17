//cpp
#include "order.h"
OrderProbe::~OrderProbe(){}
int OrderProbe::Zeta(){return mSecond;}
int OrderProbe::Alpha(){return mFirst;}
OrderProbe* MakeOrderProbe(){return new OrderProbe;}
