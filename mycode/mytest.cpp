//first.cpp Adding numbers using two nodes C++ version
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "mpi.h"
using namespace std;

typedef unsigned long ULONG;

ULONG myplus(ULONG start_index,ULONG loopTimes,ULONG leap)
{
    ULONG numCount=0;

   for(ULONG i=start_index;i<loopTimes;i=i+leap)
   {
       cout<<"i----"<<i;
       if(i%2==0)
       {
           numCount = numCount++;
       }

   }
         return numCount;
}
//always use argc and argv, as mpirun will pass the appropriate parms.
int main(int argc,char* argv[])
{
    MPI::Init(argc,argv);

    // What is my ID and how many processes are in this pool?
    int myid = MPI::COMM_WORLD.Get_rank();
    int numproc = MPI::COMM_WORLD.Get_size();
    std::cout << "This is id " << myid << " out of " << numproc << std::endl;

    ULONG finalResult=0;

    if (myid == 0) {

        // Get the number the user wants
        ULONG N = atoi(argv[1]);

        // Master sends 'N' to slave
        MPI::COMM_WORLD.Send(&N, 1, MPI::LONG, 1,0);


       finalResult = finalResult + myplus(0,N/numproc,numproc);

        ULONG slaveCount=0;

        MPI::COMM_WORLD.Recv(&slaveCount, 1, MPI::LONG, 1,0);
        finalResult = finalResult + slaveCount;

        std::cout << " 偶数一共有： " << finalResult << std::endl;
    }

    else if (myid == 1) {

        // Slave waits to receive 'N' from master
        ULONG N;
        MPI::COMM_WORLD.Recv(&N, 1, MPI::LONG, 0, 0);
        ULONG scount = myplus(myid,N/numproc,numproc);

        // Slave sends 'sum1' to master
        MPI::COMM_WORLD.Send(&scount, 1, MPI::LONG, 0, 0);
    }
    MPI::Finalize();
}
