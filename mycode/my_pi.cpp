#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "mpi.h"

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
    MPI::Init(argc,argv);

    // What is my ID and how many processes are in this pool?
    int myid = MPI::COMM_WORLD.Get_rank();
    int numproc = MPI::COMM_WORLD.Get_size();
    std::cout << "This is id " << myid << " out of " << numproc << std::endl;

    if (myid == 0) {

        // Get the number the user wants
        int N = atoi(argv[1]);

        // Master sends 'N' to slave
        MPI::COMM_WORLD.Send(&N, 1, MPI::INT, 1,0);

        // Partial result for node 0
        int sum0 = 0;
        for(int i = 1; i <= N/2; i++){
            sum0 = sum0 + i;
        }

        //Master waits to receive 'sum1' from slave
        int sum1;
        MPI::COMM_WORLD.Recv(&sum1, 1, MPI::INT, 1,0);
        int result = sum0 + sum1;

        std::cout << "The final result is " << result << std::endl;
    }

    return 0;
}