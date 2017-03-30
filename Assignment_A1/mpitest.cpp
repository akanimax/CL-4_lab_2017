/* program hello */
/* Adapted from mpihello.f by drs */

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
using namespace std;
int main(int argc, char **argv)
{
  int rank;
  char hostname[256];
  //printf("H\n");
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  gethostname(hostname,255);

  cout<<"Hello world!  I am process number: "<<rank<<" on host "<<hostname<<"\n";

  MPI_Finalize();

  return 0;
}
