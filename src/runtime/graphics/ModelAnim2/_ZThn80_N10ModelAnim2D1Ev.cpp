//cpp
struct Base1 { int pad[19]; virtual ~Base1(); virtual void f1(); };
struct Base2 { int b; virtual ~Base2(); virtual void g1(); };
struct ModelAnim2 : Base1, Base2 { virtual ~ModelAnim2(); };
ModelAnim2::~ModelAnim2() {}
