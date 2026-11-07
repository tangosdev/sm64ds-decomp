extern int data_02099e20;
/* Set the channel measurement's state word. As the callers use it: 0
   initializing WM (func_0203f9d4), 1 measuring (func_0203f8a4), 2 shutting
   WM down (func_0203f6e8), 3 idle, set when WM_End completes
   (func_0203f920). func_0203f9d4 refuses to start unless it reads 3. */
void func_0203f6a4(int v) {
    data_02099e20 = v;
}
