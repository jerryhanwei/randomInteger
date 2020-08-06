#include <iostream>
using namespace std;

typedef long long ULONG;

const ULONG a = 1664525;
const ULONG c = 1013904223;

const ULONG m = 4294967296;
const ULONG sidelen = 65536; // sqrt of m
ULONG fallInCircleTimes=0;

double getNewDistance(double x, double y)
{
    return (x * x + y * y);
}

// Evaluate (ax+c) mod m
ULONG modlin(ULONG a, ULONG x, ULONG c, ULONG m)
{
    return (a*x + c) % m;
}
// Put integer n in range x1,x2 with the maximum integer value
double rescale(ULONG N, ULONG n, double x1, double x2)
{
    double f = static_cast<double>(n) / static_cast<double>(N);
    return x1 + f*(x2 - x1);
}

void checkPointInCircle(ULONG startIndex,ULONG loopTimes,int leapConst) {
    ULONG i_prev = 12345; // Seed value
    for (ULONG n = startIndex; n < loopTimes; n = n + leapConst) {
        cout<<"startIndex:--"<<startIndex<<endl;
        cout<<"n:--"<<n<<endl;
        ULONG i_next = modlin(a, i_prev, c, m);
        i_prev = i_next;
// Scale the random number to a random 2−d position
        ULONG ix = i_next % sidelen;
        ULONG iy = i_next / sidelen;
// Scale current random integer to value from 0−1
        double x = rescale(sidelen, ix, -1, 1);
        double y = rescale(sidelen, iy, -1, 1);

        if (getNewDistance(x, y) <= 1) {
            fallInCircleTimes++;
        }
    }
}

int main(int argc,char* argv[])
{
    checkPointInCircle(0,12,4);
    cout<<"击中总次数："<<fallInCircleTimes;
    return 0;
}