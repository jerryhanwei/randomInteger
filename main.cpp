#include <iostream>
using namespace std;

typedef long long ULONG;

const ULONG a = 1664525;
const ULONG c = 1013904223;

const ULONG m = 4294967296;
const ULONG sidelen = 65536; // sqrt of m

ULONG myplus(ULONG start_index,ULONG loopTimes,ULONG leap)
{
    ULONG numCount=0;

    for(ULONG i=start_index;i<loopTimes;i=i+leap)
    {
        cout<<"i----"<<i<<endl;
        if(i<5)
        {
            numCount = numCount++;
        }

    }
    return numCount;
}

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

ULONG checkPointInCircle(ULONG startIndex,ULONG loopTimes,int leapConst) {
    ULONG i_prev = 12345; // Seed value
    ULONG fallInCircleTimes=0;
    for (ULONG n = startIndex; n < loopTimes; n = n + leapConst) {
       // cout<<"startIndex:--"<<startIndex<<endl;
       // cout<<"n:--"<<n<<endl;
        ULONG i_next = modlin(a, i_prev, c, m);
        i_prev = i_next;
// Scale the random number to a random 2−d position
        ULONG ix = i_next % sidelen;
        ULONG iy = i_next / sidelen;
// Scale current random integer to value from 0−1
        double x = rescale(sidelen, ix, -1, 1);
        double y = rescale(sidelen, iy, -1, 1);


        if (getNewDistance(x, y) <= 1) {
            fallInCircleTimes = fallInCircleTimes++;
        }
    }

    return fallInCircleTimes;
}

int main(int argc,char* argv[])
{
    //checkPointInCircle(0,10,2);

   // cout<<"击中总次数："<<checkPointInCircle(0,1000,1);

    ULONG test = myplus(0,10,1);
    cout<<test<<endl;

   return 0;
}