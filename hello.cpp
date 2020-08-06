#include <iostream>

typedef unsigned long ULONG;

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
int main(int argc, char * argv[])
{
// For the sequential random number generator
    const ULONG a = 1664525;
    const ULONG c = 1013904223;
    const ULONG m = 4294967296;
    const ULONG sidelen = 65536; // sqrt of m
    const ULONG numtrials = 10;
    ULONG i_prev = 12345; // Seed value
    for (ULONG n = 0; n < numtrials; ++n) {
        ULONG i_next = modlin(a, i_prev, c, m);
        i_prev = i_next;
// Scale the random number to a random 2−d position
        ULONG ix = i_next%sidelen;
        ULONG iy = i_next/sidelen;
// Scale current random integer to value from 0−1
        double x = rescale(sidelen, ix, -1, 1);
        double y = rescale(sidelen, iy, -1, 1);

        return 0;
    }
}
