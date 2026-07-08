extern int PROCESS_STATE;
extern int data_020a4ba8, data_020a4b88, FIRST_BEHAVIOR_LIST_NODE, FIRST_RENDER_LIST_NODE, data_020a4b6c;
extern void func_02043fdc(int *p);
extern void func_020441cc(int *p);

void func_02044120(void)
{
    PROCESS_STATE = 4;
    func_02043fdc(&data_020a4ba8);
    PROCESS_STATE = 2;
    func_02043fdc(&data_020a4b88);
    PROCESS_STATE = 3;
    func_02043fdc(&FIRST_BEHAVIOR_LIST_NODE);
    PROCESS_STATE = 5;
    func_02043fdc(&FIRST_RENDER_LIST_NODE);
    PROCESS_STATE = 1;
    func_020441cc(&data_020a4b6c);
    PROCESS_STATE = 0;
}
