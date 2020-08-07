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

long test1(int start,int N,int leap)
{
    cout<<"start:"<<start<<"-----";
    // Partial result for node 0
    long sum0 = 0;
    for(long i = start; i <= N; i=i+leap){
        cout<<"i-----"<<i<<endl;
        sum0 = sum0 + i;
    }
    //cout<<"sum0 for:"<<sum0<<endl;
    return sum0;

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
        long N = atoi(argv[1]);

        // Master sends 'N' to slave

        for(int i=1;i<numproc;i++)
        {
            MPI::COMM_WORLD.Send(&N, 1, MPI_LONG, i,0);
        }


        long sum0 = test1(myid,N,numproc);

        //Master waits to receive 'sum1' from slave
        long sum1;
        long slaveSum=0;
        long result;
        for(int i=1;i<numproc;i++)
        {
            MPI::COMM_WORLD.Recv(&sum1, 1, MPI_LONG, i,0);
            slaveSum = slaveSum+sum1;
            cout<<"sum1--for-slave:"<<sum1<<endl;
            cout<<"sum1--for-slaveSum:"<<slaveSum<<endl;
        }
         cout<<"sum0 final:"<<sum0<<endl;
        cout<<"slaveSum final:"<<slaveSum<<endl;
        result = sum0 + slaveSum;


        std::cout << "The final result is " << result << std::endl;
    }

    else{

        // Slave waits to receive 'N' from master
        long N;
        MPI::COMM_WORLD.Recv(&N, 1, MPI_LONG, 0, 0);
        long sum1 = 0;
        sum1 = test1(myid,N,numproc);

        // Slave sends 'sum1' to master
        MPI::COMM_WORLD.Send(&sum1, 1, MPI_LONG, 0, 0);
    }
    MPI::Finalize();
}