#include <iostream>
#include <string>
#include <fstream>
#include "Parse_c.h"

using namespace std;
int main()
{
    string s("J:\\Desktop\\xiangmu\\cyuyanceshi\\my_print.c");
    Parse_c c(s, cin);
    c.show_mem();
    return 0;
}
