#define OP_1 3
#define OP_2 4

int operate();
inline int operate_inline() __attribute__((always_inline)); //tell gcc to inline

int main(void)
{
    operate();
    int q = operate_inline();
    return !q;
}

int operate()
{
    int op1 = OP_1;
    int op2 = OP_2;
    return op1 * op2;
}

int operate_inline()
{
    int op1 = OP_1;
    int op2 = OP_2;
    return op1 * op2;
}