#include <iostream>
#include "biginteger.h"
using namespace std;

int main() {
    BigInteger a, b;

    cout << "========= Input Values =========" << endl;
    cin >> a >> b;
    cout << endl;

    cout << "========= Values =========" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << endl;

    cout << "========= Binary Operator =========" << endl;
    cout << "a + b = " << a + b << endl;
    cout << "a - b = " << a - b << endl;
    cout << "a * b = " << a * b << endl;
    cout << "a / b = " << a / b << endl;
    cout << "a % b = " << a % b << endl;
    cout << endl;

    cout << "========= Relational Operator =========" << endl;
    cout << boolalpha;
    cout << "a == b is " << (a == b) << endl;
    cout << "a != b is " << (a != b) << endl;
    cout << "a < b is " << (a < b) << endl;
    cout << "a > b is " << (a > b) << endl;
    cout << "a <= b is " << (a <= b) << endl;
    cout << "a >= b is " << (a >= b) << endl;
    cout << endl;

    cout << "========= Logical Operator =========" << endl;
    cout << "a || b is " << (a || b) << endl;
    cout << "a && b is " << (a && b) << endl;
    cout << "!a is " << !a << endl;
    cout << endl;

    cout << "========= Unary Operator =========" << endl;
    cout << "+a = " << +a << endl;
    cout << "-a = " << -a << endl;
    cout << endl;

    cout << "========= Increment / Decrement Operator =========" << endl;
    cout << "a++ is " << a++ << endl;
    cout << "a = " << a << endl;
    cout << endl;

    cout << "++a is " << ++a << endl;
    cout << "a = " << a << endl;
    cout << endl;

    cout << "--a is " << --a << endl;
    cout << "a = " << a << endl;
    cout << endl;

    cout << "a-- is " << a-- << endl;
    cout << "a = " << a << endl;
    cout << endl;

    cout << "========= Assignment Operator =========" << endl;
    a += b;
    cout << "after a += b" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << endl;

    a -= b;
    cout << "after a -= b" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << endl;

    a *= b;
    cout << "after a *= b" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << endl;

    a /= b;
    cout << "after a /= b" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << endl;

    a %= b;
    cout << "after a %= b" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << endl;

    return 0;
}
