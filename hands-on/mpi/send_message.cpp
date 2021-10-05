#include <iostream>
#include "mpi.h"

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
    dest = 1;
    MPI_Ssend(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    std::cout << "Rank 0 successfully sent a message to Rank 1: " << outmsg << std::endl;
  }
  // rank 1 waits for rank 0 message then returns a message
  else if (rank == 1) {
    std::cout << "Rank 1 is waiting for a message from Rank 0" << std::endl;
    source = 0;
    MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    std::cout << "Rank 1 successfully received a message from Rank 0: " << inmsg << std::endl;
  }
  MPI_Finalize();
}
