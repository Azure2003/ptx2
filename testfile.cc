/*  Copyright (C) 2004 University of Oxford  */

/*  CCOPYRIGHT  */


#include "csv_mesh.h"
#include "streamlines.h"


int main(int argc, char** argv){
  int Nrows=241323;
  int Ncols=32492;

  SpMat_HCP M(Nrows,Ncols);
  cout<<"Read"<<endl;
  M.LoadTrajFile("/home/fs0/saad/scratch/test_matrix4/test/fdt_matrix4_");
  M.Print();
  cout<<"Write"<<endl;
  M.SaveTrajFile("/home/fs0/saad/scratch/test_matrix4/test/copy_of_matri4_");

  return 0;
}


