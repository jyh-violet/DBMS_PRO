
#include "storage/dataType.h"

int main()
{
    vmlog(true, "------ <test_datatype.c> begining ------");
    char *iStr = "12";
    char *str  = "23.123";
    int32 iInt   = char2Int(iStr);
    float iFloat   = char2Float(str);
    double iDouble = char2Double(str);

    vmlog(true, "------ iStr=%s,iInt=%d", iStr, iInt);
    vmlog(true, "------ str=%s,iFloat=%f,iDouble=%lf", str, iFloat, iDouble);
    vmlog(true, "------ <test_datatype.c> ending ------");
    
    return 0;
}


