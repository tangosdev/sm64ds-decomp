struct Node5b1a4 { struct Node5b1a4 *next; };

extern struct Node5b1a4 *data_020a6490;
extern struct Node5b1a4 *data_020a648c;

void func_0205b1a4(struct Node5b1a4 *node)
{
    if (data_020a6490 == 0) {
        data_020a6490 = node;
        data_020a648c = node;
    } else {
        data_020a6490->next = node;
        data_020a6490 = node;
    }
    node->next = 0;
}
