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
