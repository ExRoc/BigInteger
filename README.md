# C++ 实现 `BigInteger` 类

## 概要

本仓库提供可复用 C++ `BigInteger` 类的声明与实现，本人希望能提供“像使用基本数据类型一样方便”的高精度整数类，该类具有以下特点：

### 优点

1. 内部实现依赖 `vector<int>` 类型，理论上支持无穷多位高精度整数的计算，上限取决于计算机内存；
2. 重载大量运算符，支持多种类型构造函数，使用流畅，体验较好；
3. 基于底层实现提供更高效的基础函数（见“[支持的其他函数](#supported_other_function)”一节）；
4. 每 4 字节存 8 个 10 进制位，效率是大多数网上 C++ 高精度整数类实现的 8 倍。

### 缺点

1. 未支持位运算符，不能与 `int` / `long long` 等基础数据类型无缝切换，涉及到多位位运算建议使用 STL 官方支持的 [std::bitset](http://www.cplusplus.com/reference/bitset/bitset/)；
2. 计算速度无法与语言原生类型相比，在 `__int128` 范围内建议使用 C++ 提供的基础数据类型进行计算，不推荐使用 `BigInteger` 类；
3. 乘法计算实现未使用速度更快的 FFT 算法实现，存在优化空间；
4. 更多功能局限性见[其他说明](#others)。

## 已重载运算符

`BigInteger` 类重载了除位运算外所有与整数计算相关的运算符，目前已重载运算符如下表：

|   运算符类型   | 运算符                                                       |
| :------------: | ------------------------------------------------------------ |
|   双目运算符   | + (加), - (减), * (乘), / (整除), % (取模)                   |
|   关系运算符   | == (等于), != (不等于), < (小于), > (大于), <= (小于等于), >=(大于等于) |
|   逻辑运算符   | \|\| (逻辑或), && (逻辑与), ! (逻辑非)                       |
|   单目运算符   | + (正), - (负)                                               |
| 自增自减运算符 | ++ (自增)， -- (自减)                                        |
|   赋值运算符   | =, +=, -=, *=, /=, %=                                        |
|    位运算符    | >> (右移运算符，与输入流关联), << (左移运算符，与输出流关联) |

使用方式见[附录 2: 重载运算符](#overwrite_operator)。

## <a id="supported_other_function">支持的其他函数</a>

在使用 `vector<int>` 作为底层数据结构存储的情况下，`BigInteger` 类针对某些特殊场景提供了更高效的成员函数供外部调用：

|            函数声明            | 函数功能                                   |
| :----------------------------: | ------------------------------------------ |
|     `size_t size() const`      | 返回 `BigInteger` 的十进制位数             |
| `BigInteger e(size_t n) const` | 返回 `BigInteger` 对象 $\times10^n$ 后的值 |
|    `BigInteger abs() const`    | 返回 `BigInteger` 对象的绝对值             |

使用方式见[附录 3: 其他函数](#other_function)，更多函数请使用者在已有代码基础上自行编写。

## <a id="performance">运行性能</a>

为了评测 `BigInteger` 类的性能，这里将 `BigIntger` 类与基本数据类型 `int` / `long long` 的计算速度进行对比，随机生成 80000 组数据，计算得到 `BigInteger` 类与基本数据类型进行 80000 次相同计算的耗时比：（$\frac{t_{BigInteger}}{t_{int~or~long~long}}$），部分基础能力评测结果如下表：

|                   运算                    |   int   | long long |
| :---------------------------------------: | :-----: | :-------: |
| `ostream& operator<<(ostream&, const T&)` | 1.0628  |  1.05479  |
|    `istream& operator>>(istream&, T&)`    | 1.63793 |  1.47059  |
|                  `abs()`                  | 1.2542  |  1.28082  |
|                比较运算符                 | 1.36558 |  1.33571  |
|     `T operator+(const T&, const T&)`     | 2.68966 |  2.55204  |
|     `T operator-(const T&, const T&)`     | 2.48848 |  2.59545  |
|     `T operator*(const T&, const T&)`     | 1.7549  |  1.89862  |
|     `T operator/(const T&, const T&)`     | 6.4381  |  15.0192  |
|     `T operator%(const T&, const T&)`     | 8.70732 |  15.9469  |

检验结果准确性与运行时间代码见[附录 4: 性能测试](#performance_testing)。默认将 `BigInteger` 类的计算时间与 `int` 类型数据计算时间比较，若要与 `long long` 类型计算时间进行比较，可将程序中

```cpp
typedef int Type
```
　　改为

```cpp
typedef long long Type
```

## <a id="others">其他说明</a>

此为第二版本，修复初版以下问题：

1. 缺少 `const BigInteger& operator=(int n)` 赋值函数导致的程序二义性问题
2. 对于常量 `LONG_LONG_MIN` abs 为负值的修正
3. 添加输入与构造函数的 `const char*` 格式检查，格式不符时，当前输入失效，不改变变量值，构造函数则默认构造 `BigInteger(0)`

该类的局限性：

1. 可以与 `bool` / `int` / `long long` 数据类型做隐式类型转换（只能从低精度往高精度），但不能与浮点数做隐式类型转换
2. 构造函数不支持 `long int` 类型（也无计划支持）
3. 对除以 0 错误不做处理

## 附录

### <a id="source_code">附录 1: 源代码</a>

#### biginteger.h 头文件：

```cpp
#ifndef BIGINTEGER_H_
#define BIGINTEGER_H_

#include <vector>
#include <iostream>
#include <cstring>
#include <iomanip>
using namespace std;

class BigInteger {
private:
    static const int BASE = 100000000;
    static const int WIDTH = 8;
    bool sign;
    size_t length;
    vector<int> num;

    void cutLeadingZero();
    void setLength();

public:
    static const long long LONG_LONG_MIN = 1LL << 63;

    BigInteger(int n = 0);
    BigInteger(long long n);
    BigInteger(const char *n);
    BigInteger(const BigInteger &n);

    const BigInteger& operator=(int n);
    const BigInteger& operator=(long long n);
    const BigInteger& operator=(const char *n);
    const BigInteger& operator=(const BigInteger &n);

    size_t size() const;
    BigInteger e(size_t n) const;
    BigInteger abs() const;

    const BigInteger& operator+() const;
    friend BigInteger operator+(const BigInteger &a, const BigInteger &b);
    const BigInteger& operator+=(const BigInteger &n);
    const BigInteger& operator++();
    BigInteger operator++(int);

    BigInteger operator-() const;
    friend BigInteger operator-(const BigInteger &a, const BigInteger &b);
    const BigInteger& operator-=(const BigInteger &n);
    const BigInteger& operator--();
    BigInteger operator--(int);

    friend BigInteger operator*(const BigInteger &a, const BigInteger &b);
    const BigInteger& operator*=(const BigInteger &n);

    friend BigInteger operator/(const BigInteger &a, const BigInteger &b);
    const BigInteger& operator/=(const BigInteger &n);

    friend BigInteger operator%(const BigInteger &a, const BigInteger &b);
    const BigInteger& operator%=(const BigInteger &n);

    friend bool operator<(const BigInteger &a, const BigInteger &b);
    friend bool operator<=(const BigInteger &a, const BigInteger &b);
    friend bool operator>(const BigInteger &a, const BigInteger &b);
    friend bool operator>=(const BigInteger &a, const BigInteger &b);
    friend bool operator==(const BigInteger &a, const BigInteger &b);
    friend bool operator!=(const BigInteger &a, const BigInteger &b);

    friend bool operator||(const BigInteger &a, const BigInteger &b);
    friend bool operator&&(const BigInteger &a, const BigInteger &b);
    bool operator!();

    friend ostream& operator<<(ostream &out, const BigInteger &n);
    friend istream& operator>>(istream &in, BigInteger &n);
};

#endif // BIGINTEGER_H_
```

#### biginteger.cpp 文件：

```cpp
#include "biginteger.h"

void BigInteger::cutLeadingZero() {
    while(num.back() == 0 && num.size() != 1) {
        num.pop_back();
    }
}

void BigInteger::setLength() {
    cutLeadingZero();
    int tmp = num.back();
    if(tmp == 0) {
        length = 1;
    } else {
        length = (num.size() - 1) * 8;
        while(tmp > 0) {
            ++length;
            tmp /= 10;
        }
    }
}

BigInteger::BigInteger(int n) {
    *this = n;
}

BigInteger::BigInteger(long long n) {
    *this = n;
}

BigInteger::BigInteger(const char *n) {
    *this = n;
}

BigInteger::BigInteger(const BigInteger &n) {
    *this = n;
}

const BigInteger& BigInteger::operator=(int n) {
    *this = (long long)n;
    return *this;
}

const BigInteger& BigInteger::operator=(long long n) {
    num.clear();
    if(n == 0) {
        num.push_back(0);
    }
    if(n >= 0) {
        sign = true;
    } else if(n == LONG_LONG_MIN) {
        *this = "-9223372036854775808";
        return *this;
    } else if(n < 0) {
        sign = false;
        n = -n;
    }
    while(n != 0) {
        num.push_back(n % BASE);
        n /= BASE;
    }
    setLength();
    return *this;
}

const BigInteger& BigInteger::operator=(const char *n) {
    int len = strlen(n);
    int tmp = 0;
    int ten = 1;
    int stop = 0;
    num.clear();
    sign = (n[0] != '-');
    if(!sign) {
        stop = 1;
    }
    for(int i = len; i > stop; --i) {
        tmp += (n[i - 1] - '0') * ten;
        ten *= 10;
        if((len - i) % 8 == 7) {
            num.push_back(tmp);
            tmp = 0;
            ten = 1;
        }
    }
    if((len - stop) % WIDTH != 0) {
        num.push_back(tmp);
    }
    setLength();
    return *this;
}

const BigInteger& BigInteger::operator=(const BigInteger &n) {
    num = n.num;
    sign = n.sign;
    length = n.length;
    return *this;
}

size_t BigInteger::size() const {
    return length;
}

BigInteger BigInteger::e(size_t n) const {
    int tmp = n % 8;
    BigInteger ans;
    ans.length = n + 1;
    n /= 8;
    while(ans.num.size() <= n) {
        ans.num.push_back(0);
    }
    ans.num[n] = 1;
    while(tmp--) {
        ans.num[n] *= 10;
    }
    return ans * (*this);
}

BigInteger BigInteger::abs() const {
    BigInteger ans(*this);
    ans.sign = true;
    return ans;
}

const BigInteger& BigInteger::operator+() const {
    return *this;
}

BigInteger operator+(const BigInteger &a, const BigInteger &b) {
    if(!b.sign) {
        return a - (-b);
    }
    if(!a.sign) {
        return b - (-a);
    }
    BigInteger ans;
    int carry = 0;
    int aa, bb;
    size_t lena = a.num.size();
    size_t lenb = b.num.size();
    size_t len = max(lena, lenb);
    ans.num.clear();
    for(size_t i = 0; i < len; ++i) {
        if(lena <= i) {
            aa = 0;
        } else {
            aa = a.num[i];
        }
        if(lenb <= i) {
            bb = 0;
        } else {
            bb = b.num[i];
        }
        ans.num.push_back((aa + bb + carry) % BigInteger::BASE);
        carry = (aa + bb + carry) / BigInteger::BASE;
    }
    if(carry > 0) {
        ans.num.push_back(carry);
    }
    ans.setLength();
    return ans;
}

const BigInteger& BigInteger::operator+=(const BigInteger &n) {
    *this = *this + n;
    return *this;
}

const BigInteger& BigInteger::operator++() {
    *this = *this + 1;
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger ans(*this);
    *this = *this + 1;
    return ans;
}

BigInteger BigInteger::operator-() const {
    BigInteger ans(*this);
    if(ans != 0) {
        ans.sign = !ans.sign;
    }
    return ans;
}

BigInteger operator-(const BigInteger &a, const BigInteger &b) {
    if(!b.sign) {
        return a + (-b);
    }
    if(!a.sign) {
        return -((-a) + b);
    }
    if(a < b) {
        return -(b - a);
    }
    BigInteger ans;
    int carry = 0;
    int aa, bb;
    size_t lena = a.num.size();
    size_t lenb = b.num.size();
    size_t len = max(lena, lenb);
    ans.num.clear();
    for(size_t i = 0; i < len; ++i) {
        aa = a.num[i];
        if(i >= lenb) {
            bb = 0;
        } else {
            bb = b.num[i];
        }
        ans.num.push_back((aa - bb - carry + BigInteger::BASE) % BigInteger::BASE);
        if(aa < bb + carry) {
            carry = 1;
        } else {
            carry = 0;
        }
    }
    ans.setLength();
    return ans;
}

const BigInteger& BigInteger::operator-=(const BigInteger &n) {
    *this = *this - n;
    return *this;
}

const BigInteger& BigInteger::operator--() {
    *this = *this - 1;
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger ans(*this);
    *this = *this - 1;
    return ans;
}

BigInteger operator*(const BigInteger &a, const BigInteger&b) {
    size_t lena = a.num.size();
    size_t lenb = b.num.size();
    vector<long long> ansLL;
    for(size_t i = 0; i < lena; ++i) {
        for(size_t j = 0; j < lenb; ++j) {
            if(i + j >= ansLL.size()) {
                ansLL.push_back((long long)a.num[i] * (long long)b.num[j]);
            } else {
                ansLL[i + j] += (long long)a.num[i] * (long long)b.num[j];
            }
        }
    }
    while(ansLL.back() == 0 && ansLL.size() != 1) {
        ansLL.pop_back();
    }
    size_t len = ansLL.size();
    long long carry = 0;
    long long tmp;
    BigInteger ans;
    ans.sign = (ansLL.size() == 1 && ansLL[0] == 0) || (a.sign == b.sign);
    ans.num.clear();
    for(size_t i = 0; i < len; ++i) {
        tmp = ansLL[i];
        ans.num.push_back((tmp + carry) % BigInteger::BASE);
        carry = (tmp + carry) / BigInteger::BASE;
    }
    if(carry > 0) {
        ans.num.push_back(carry);
    }
    ans.setLength();
    return ans;
}

const BigInteger& BigInteger::operator*=(const BigInteger &n) {
    *this = *this * n;
    return *this;
}

BigInteger operator/(const BigInteger &a, const BigInteger &b) {
    BigInteger aa(a.abs());
    BigInteger bb(b.abs());
    if(aa < bb) {
        return 0;
    }
    char *str = new char[aa.size() + 1];
    memset(str, 0, sizeof(char) * (aa.size() + 1));
    BigInteger tmp;
    int lena = aa.length;
    int lenb = bb.length;
    for(int i = 0; i <= lena - lenb; ++i) {
        tmp = bb.e(lena - lenb - i);
        while(aa >= tmp) {
            ++str[i];
            aa = aa - tmp;
        }
        str[i] += '0';
    }

    BigInteger ans(str);
    delete[]str;
    ans.sign = (ans == 0 || a.sign == b.sign);
    return ans;
}

const BigInteger& BigInteger::operator/=(const BigInteger &n) {
    *this = *this / n;
    return *this;
}

BigInteger operator%(const BigInteger &a, const BigInteger &b) {
    return a - a / b * b;
}

const BigInteger& BigInteger::operator%=(const BigInteger &n) {
    *this = *this - *this / n * n;
    return *this;
}

bool operator<(const BigInteger &a, const BigInteger &b) {
    if(a.sign && !b.sign) {
        return false;
    } else if(!a.sign && b.sign) {
        return true;
    } else if(a.sign && b.sign) {
        if(a.length < b.length) {
            return true;
        } else if(a.length > b.length) {
            return false;
        } else {
            size_t lena = a.num.size();
            for(int i = lena - 1; i >= 0; --i) {
                if(a.num[i] < b.num[i]) {
                    return true;
                } else if(a.num[i] > b.num[i]) {
                    return false;
                }
            }
            return false;
        }
    } else {
        return -b < -a;
    }
}

bool operator<=(const BigInteger &a, const BigInteger &b) {
    return !(b < a);
}

bool operator>(const BigInteger &a, const BigInteger &b) {
    return b < a;
}

bool operator>=(const BigInteger &a, const BigInteger &b) {
    return !(a < b);
}

bool operator==(const BigInteger &a, const BigInteger &b) {
    return !(a < b) && !(b < a);
}

bool operator!=(const BigInteger &a, const BigInteger &b) {
    return (a < b) || (b < a);
}

bool operator||(const BigInteger &a, const BigInteger &b) {
    return a != 0 || b != 0;
}

bool operator&&(const BigInteger &a, const BigInteger &b) {
    return a != 0 && b != 0;
}

bool BigInteger::operator!() {
    return *this == 0;
}

ostream& operator<<(ostream &out, const BigInteger &n) {
    size_t len = n.num.size();
    if(!n.sign) {
        out << '-';
    }
    out << n.num.back();
    for(int i = len - 2; i >= 0; --i) {
        out << setw(BigInteger::WIDTH) << setfill('0') << n.num[i];
    }
    return out;
}

istream& operator>>(istream &in, BigInteger &n) {
    string str;
    in >> str;
    size_t len = str.length();
    size_t i, start = 0;
    if(str[0] == '-') {
        start = 1;
    }
    if(str[start] == '\0') {
        return in;
    }
    for(i = start; i < len; ++i) {
        if(str[i] < '0' || str[i] > '9') {
            return in;
        }
    }
    n = str.c_str();
    return in;
}
```

[返回正文 ↑](#summary)

### <a id="overwrite_operator">附录 2: 重载运算符</a>

#### 使用代码示例

```cpp
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
```

#### 执行测试

假设输入 a b 值分别为 123456789012345678901234567890 与 987654321098765432109876543210，则控制台展示如下：

```bash
========= Input Values =========
123456789012345678901234567890
987654321098765432109876543210

========= Values =========
a = 123456789012345678901234567890
b = 987654321098765432109876543210

========= Binary Operator =========
a + b = 1111111110111111111011111111100
a - b = -864197532086419753208641975320
a * b = 121932631137021795226185032733622923332237463801111263526900
a / b = 0
a % b = 123456789012345678901234567890

========= Relational Operator =========
a == b is false
a != b is true
a < b is true
a > b is false
a <= b is true
a >= b is false

========= Logical Operator =========
a || b is true
a && b is true
!a is false

========= Unary Operator =========
+a = 123456789012345678901234567890
-a = -123456789012345678901234567890

========= Increment / Decrement Operator =========
a++ is 123456789012345678901234567890
a = 123456789012345678901234567891

++a is 123456789012345678901234567892
a = 123456789012345678901234567892

--a is 123456789012345678901234567891
a = 123456789012345678901234567891

a-- is 123456789012345678901234567891
a = 123456789012345678901234567890

========= Assignment Operator =========
after a += b
a = 1111111110111111111011111111100
b = 987654321098765432109876543210

after a -= b
a = 123456789012345678901234567890
b = 987654321098765432109876543210

after a *= b
a = 121932631137021795226185032733622923332237463801111263526900
b = 987654321098765432109876543210

after a /= b
a = 123456789012345678901234567890
b = 987654321098765432109876543210

after a %= b
a = 123456789012345678901234567890
b = 987654321098765432109876543210


Process returned 0 (0x0)   execution time : 24.289 s
Press any key to continue.
```

[返回正文 ↑](#overwrited_operator)

### <a id="other_function">附录 3: 其他函数</a>

#### 使用代码示例

```cpp
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
```

#### 执行测试

```bash
a = -123456789012345678901234567890
a.size() = 30
a.e(5) = -12345678901234567890123456789000000
a.abs() = 123456789012345678901234567890
a.abs().e(2) = 12345678901234567890123456789000

Process returned 0 (0x0)   execution time : 0.043 s
Press any key to continue.
```

[返回正文 ↑](#supported_other_function)

### <a id="performance_testing">附录 4: 性能测试</a>

#### 测试代码

```cpp
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <windows.h>
#include "biginteger.h"
using namespace std;

typedef long long Type;
const int TESTS = 80000;
const char typefile[] = "type.txt";
const char bigfile[] = "big.txt";
const Type ZeroType = 0;
const BigInteger ZeroBig = 0;

Type type[TESTS + 5];
BigInteger big[TESTS + 5];

void Rand(bool half = false, bool Sqrt = false);
void Output();
void Input();
void Absolute();
void Compare_Calculate();

int main() {
    ios::sync_with_stdio(false);
    srand((unsigned)time(NULL));
    Rand();
    Output();
    Input();
    Absolute();
    Compare_Calculate();
    return 0;
}

void Rand(bool half, bool Sqrt) {
    Type tmp;
    for (int i = 0; i < TESTS; ++i) {
        do {
            tmp = ((Type)rand()) << (rand() % (sizeof(Type) * 4));
            if(rand() % 2 == 1) {
                tmp *= ((Type)rand()) << (rand() % (sizeof(Type) * 4));
            }
            if(half) {
                tmp = abs(tmp / 2) - 1;
            }
            if(Sqrt) {
                if(abs(tmp) < 0) {
                    --tmp;
                }
                tmp = sqrt(abs(tmp)) - 1;
            }
            if(rand() % 2 == 1) {
                tmp = -tmp;
            }
        } while(tmp == 0);
        type[i] = tmp;
        big[i] = tmp;
    }
}

void Output() {
    ofstream fout;
    int cnt;
    clock_t start, stop;
    double time_of_type, time_of_big;

    fout.open(typefile);
    cnt = 0;
    start = clock();
    while(cnt != TESTS) {
        fout << type[cnt++] << endl;
    }
    fout << ZeroType << endl;
    stop = clock();
    fout.close();
    time_of_type = stop - start;

    fout.open(bigfile);
    cnt = 0;
    start = clock();
    while(cnt != TESTS) {
        fout << big[cnt++] << endl;
    }
    fout << ZeroBig << endl;
    stop = clock();
    fout.close();
    time_of_big = stop - start;

    cout << "Output time of type: " << time_of_type << endl
         << "Output time of big: " << time_of_big << endl
         << "time_of_big / time_of_type = " << time_of_big / time_of_type << endl << endl;

    system((string("fc ") + typefile + " " + bigfile).c_str());
}

void Input() {
    ifstream fin;
    Type typetmp;
    BigInteger bigtmp;
    clock_t start, stop;
    double time_of_type, time_of_big;

    fin.open(typefile);
    start = clock();
    while(fin >> typetmp);
    stop = clock();
    fin.close();
    time_of_type = stop - start;

    fin.open(bigfile);
    start = clock();
    while(fin >> bigtmp);
    stop = clock();
    fin.close();
    time_of_big = stop - start;

    cout << "Input time of type: " << time_of_type << endl
         << "Input time of big: " << time_of_big << endl
         << "time_of_big / time_of_type = " << time_of_big / time_of_type << endl << endl;
}

void Absolute() {
    int cnt;
    ofstream fout;
    clock_t start, stop;
    double time_of_type, time_of_big;

    fout.open(typefile);
    cnt = 0;
    start = clock();
    while(cnt != TESTS) {
        if(type[cnt] == BigInteger::LONG_LONG_MIN) {
            fout << "9223372036854775808" << endl;
        } else if(type[cnt] == INT_MIN) {
            fout << "2147483648" << endl;
        } else {
            fout << abs(type[cnt]) << endl;
        }
        ++cnt;
    }
    fout << abs(ZeroType) << endl;
    stop = clock();
    fout.close();
    time_of_type = stop - start;

    fout.open(bigfile);
    cnt = 0;
    start = clock();
    while(cnt != TESTS) {
        fout << big[cnt].abs() << endl;
        ++cnt;
    }
    fout << ZeroBig.abs() << endl;
    stop = clock();
    fout.close();
    time_of_big = stop - start;

    cout << "Abs time of type: " << time_of_type << endl
         << "Abs time of big: " << time_of_big << endl
         << "time_of_big / time_of_type = " << time_of_big / time_of_type << endl << endl;

    system((string("fc ") + typefile + " " + bigfile).c_str());
}

template<typename T>
void cmpcal(T *arr, int command, ofstream &fout) {
    int cnt;
    const T ZERO = 0;
    cnt = 0;
    while(cnt != TESTS) {
        switch(command) {
            case 1: {
                fout << (arr[cnt] == arr[cnt + 1]) << endl
                     << (arr[cnt] != arr[cnt + 1]) << endl
                     << (arr[cnt] < arr[cnt + 1]) << endl
                     << (arr[cnt] <= arr[cnt + 1]) << endl
                     << (arr[cnt] > arr[cnt + 1]) << endl
                     << (arr[cnt] >= arr[cnt + 1]) << endl;
                break;
            }
            case 2: fout << arr[cnt] / arr[cnt + 1] << endl; break;
            case 3: fout << arr[cnt] % arr[cnt + 1] << endl; break;
            case 4: fout << arr[cnt] + arr[cnt + 1] << endl; break;
            case 5: fout << arr[cnt] - arr[cnt + 1] << endl; break;
            case 6: fout << arr[cnt] * arr[cnt + 1] << endl; break;
            default: break;
        }
        cnt += 2;
    }
    switch(command) {
        case 2: fout << ZERO / arr[0] << endl; break;
        case 3: fout << ZERO % arr[0] << endl; break;
        case 4: fout << ZERO + arr[0] << endl << arr[0] + ZERO << endl; break;
        case 5: fout << ZERO - arr[0] << endl << arr[0] - ZERO << endl; break;
        case 6: fout << ZERO * arr[0] << endl << arr[0] * ZERO << endl; break;
        default: break;
    }
}

void Compare_Calculate() {
    ofstream fout;
    clock_t start, stop;
    double time_of_type, time_of_big;
    string command[6] = {"Compare", "Div", "Mod", "Add", "Cut", "Multiply"};

    for(int i = 1; i <= 6; ++i) {
        switch(i) {
            case 4: Rand(true, false); break;
            case 6: Rand(false, true); break;
            default: break;
        }

        fout.open(typefile);
        start = clock();
        cmpcal(type, i, fout);
        stop = clock();
        fout.close();
        time_of_type = stop - start;

        fout.open(bigfile);
        start = clock();
        cmpcal(big, i, fout);
        stop = clock();
        fout.close();
        time_of_big = stop - start;

        cout << command[i - 1] << " time of type: " << time_of_type << endl
             << command[i - 1] << " time of big: " << time_of_big << endl
             << "time_of_big / time_of_type = " << time_of_big / time_of_type << endl << endl;

        system((string("fc ") + typefile + " " + bigfile).c_str());
    }
}
```

#### 测试结果

由于数据随机生成，每次结果输出不保证完全一致。

```bash
Output time of type: 414
Output time of big: 440
time_of_big / time_of_type = 1.0628

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Input time of type: 58
Input time of big: 95
time_of_big / time_of_type = 1.63793

Abs time of type: 417
Abs time of big: 523
time_of_big / time_of_type = 1.2542

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Compare time of type: 1261
Compare time of big: 1722
time_of_big / time_of_type = 1.36558

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Div time of type: 210
Div time of big: 1352
time_of_big / time_of_type = 6.4381

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Mod time of type: 205
Mod time of big: 1785
time_of_big / time_of_type = 8.70732

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Add time of type: 203
Add time of big: 546
time_of_big / time_of_type = 2.68966

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Cut time of type: 217
Cut time of big: 540
time_of_big / time_of_type = 2.48848

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异

Multiply time of type: 204
Multiply time of big: 358
time_of_big / time_of_type = 1.7549

正在比较文件 type.txt 和 BIG.TXT
FC: 找不到差异


Process returned 0 (0x0)   execution time : 11.558 s
Press any key to continue.
```

[返回正文 ↑](#performance)