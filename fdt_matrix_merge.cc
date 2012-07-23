/*  Copyright (C) 2004 University of Oxford  */

/*  CCOPYRIGHT  */
#ifndef EXPOSE_TREACHEROUS
#define EXPOSE_TREACHEROUS
#endif

#include "miscmaths/miscmaths.h"
#include "miscmaths/SpMat.h"


using namespace MISCMATHS;

void read_file_names(const string& filename,vector<string>& filelist){
  ifstream fs(filename.c_str());
  string tmp;
  if(fs){
    fs>>tmp;
    do{
      filelist.push_back(tmp);
      fs>>tmp;
    }while(!fs.eof());
    }
  else{
    cerr<<filename<<" does not exist"<<endl;
    exit(0);
  }
}

int main(int argc, char** argv){

  if(argc<2){
    cout<<"fdt_matrix_merge <matlist> <outfile>"<<endl;
    exit(1);
  }

  vector<string> filelist;
  read_file_names(argv[1],filelist);

  cout<<"merge"<<endl<<0<<endl;
  SpMat<float> omat(filelist[0]);
  SpMat<float>* tmpmat;
  for(unsigned int i=1;i<filelist.size();i++){
    cout<<i<<endl;
    tmpmat = new SpMat<float>(filelist[i]);
    omat += *tmpmat;
    delete(tmpmat);
  }
  cout<<"save"<<endl;
  omat.Print(argv[2]);
  
  return 0;
}
