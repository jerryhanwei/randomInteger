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

long getPointCountInCircle(long startIndex,long loopTimes,int leapConst) {
    ULONG i_prev = 12345; // Seed value
    long fallInCircleTimes=0;
    for (long n = startIndex; n < loopTimes; n = n + leapConst) {
       // cout<<"startIndex:--"<<startIndex<<endl;
       // cout<<"n:--"<<n<<endl;
        long i_next = modlin(a, i_prev, c, m);
        i_prev = i_next;
// Scale the random number to a random 2−d position
        long ix = i_next % sidelen;
        long iy = i_next / sidelen;
// Scale current random integer to value from 0−1
        double x = rescale(sidelen, ix, -1, 1);
        double y = rescale(sidelen, iy, -1, 1);

        if (getNewDistance(x, y) <= 1) {
           // cout<<"x:"<<x<<",y:"<<y<<endl;
            fallInCircleTimes = fallInCircleTimes+1;
           // cout<<" for loop 落入圆内次数："<<fallInCircleTimes<<endl;
        }
    }

    return fallInCircleTimes;
}

long test1(int start,int N,int leap)
{
    cout<<"start:"<<start<<"-----"<<endl;
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
    double masterStartTime=0.0;
    double masterEndTime=0.0;

    if (myid == 0) {

        // Get the loop times
        long N = atoi(argv[1]);
        masterStartTime=MPI_Wtime();
        // main get counts of falling in circle
        long sum0 = getPointCountInCircle(myid,N,numproc);

        long sum1;
        long slaveSum=0;
        long result;
        double slaveSendingTimeStart = 0.0;
        double slaveSendingTimeEnd = 0.0;
        // if the number of processes > 1
         if(numproc>1){
             slaveSendingTimeStart = MPI_Wtime();
             // main send to slaves
             for(int i=1;i<numproc;i++)
             {
                 MPI::COMM_WORLD.Send(&N, 1, MPI_LONG, i,0);
             }
             //Master waits to receive 'sum1' from slave

             for(int i=1;i<numproc;i++)
             {
                 MPI::COMM_WORLD.Recv(&sum1, 1, MPI_LONG, i,0);
                 slaveSum = slaveSum+sum1;
                 //    cout<<"sum1--for-slave:"<<sum1<<endl;
                 //    cout<<"sum1--for-slaveSum:"<<slaveSum<<endl;
             }
             result = sum0 + slaveSum;
             slaveSendingTimeEnd = MPI_Wtime();
             // cout<<"sum0 主线程击中次数:"<<sum0<<endl;
             //cout<<"slaveSum final:"<<slaveSum<<endl;
         }else{
             result = sum0;
         }

        double PI = ((double)result/N)*4;

        std::cout << " throwing times：" << N << std::endl;
        std::cout << " targeted times：" << result << std::endl;
        std::cout << " PI ：" << PI << std::endl;

        masterEndTime=MPI_Wtime();
        double wholeTime = masterEndTime-masterStartTime;
        double communicationTime = slaveSendingTimeEnd-slaveSendingTimeStart;
        int percentOfCommunication = (int)((communicationTime/wholeTime)*100);

        cout<<"the whole process elapse time: "<<wholeTime<<endl;
        cout<<"parallel program communication time: "<<communicationTime<<endl;
        cout<<"master took "<<percentOfCommunication<<"% of wholeTime to call slaves to do some help"<<endl;

    }

    else{

        // Slave waits to receive 'N' from master
        long N;
        MPI::COMM_WORLD.Recv(&N, 1, MPI_LONG, 0, 0);
        long sum1 = 0;
        // slave count number of falling in circle
        sum1 = getPointCountInCircle(myid,N,numproc);

        // Slave sends 'sum1' to master
        MPI::COMM_WORLD.Send(&sum1, 1, MPI_LONG, 0, 0);
    }
    MPI::Finalize();
}