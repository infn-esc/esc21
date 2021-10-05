#include "mpi.h"
#include <iostream>

int main(int argc, char* argv[])
{
  int numranks, rank, dest, source, rc, count, tag = 1;
  char inmsg, outmsg = 'x';
  MPI_Status Stat; // required variable for receive routines

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // rank 0 sends to rank 1 and waits to receive a return message
  if (rank == 0) {
    dest   = 1;
    source = 1;
    MPI_Ssend(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
  }

  // rank 1 waits for rank 0 message then returns a message
  else if (rank == 1) {
    dest   = 0;
    source = 0;
    MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    MPI_Ssend(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  }

  // query receive Stat variable and print message details
  MPI_Get_count(&Stat, MPI_CHAR, &count);
  std::cout << "Rank " << rank << " Received " << count << " bytes from rank "
            << Stat.MPI_SOURCE << "  with tag " << Stat.MPI_TAG << std::endl;

  MPI_Finalize();
}
