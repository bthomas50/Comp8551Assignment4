#include <iostream>

void display_int_byval(int x);
void display_int_byref(int& x);

int main(void)
{
    int x1 = 1;
    display_int_byval(x1);
    int x2 = 2;
    display_int_byref(x2);
    return 0;
}


void display_int_byref(int& x)
{
    std::cout<<x;
}

void display_int_byval(int x)
{
    std::cout<<x;
}