#include <iostream>
#include "biginteger.h"
using namespace std;

int main() {
    BigInteger a = "-123456789012345678901234567890";
    cout << "a = " << a << endl;
    cout << "a.size() = " << a.size() << endl;
    cout << "a.e(5) = " << a.e(5) << endl;
    cout << "a.abs() = " << a.abs() << endl;
    cout << "a.abs().e(2) = " << a.abs().e(2) << endl;

    return 0;
}
