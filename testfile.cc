/*  Copyright (C) 2004 University of Oxford  */

/*  CCOPYRIGHT  */


#include "csv_mesh.h"



int main(int argc, char** argv){

  CsvMesh m;
  if (argc<2) {
    m.load("/vols/Data/HCP/Issues/GIFTI/HCP.surf.gii");
  } else {
    m.load(argv[1]);
  }
  //m.print("grot.txt");
  //m.save_ascii("grot.asc");

  //m.save("grot.asc",CSV_ASCII);
  m.save("grot.surf.gii",CSV_GIFTI);

  return 0;
}


