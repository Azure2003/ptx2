/*  Copyright (C) 2004 University of Oxford  */

/*  CCOPYRIGHT  */

#include <iostream>
#include <fstream>
#include "newimage/newimageall.h"
#include "csv_mesh.h"
#include <vector>
using namespace std;
using namespace NEWIMAGE;

void proj_thresh_volumes(const vector<string>& innames,const float& thresh){
  vector<volume<float> > tmpvec(innames.size());
  volume<float> tmp;
  cout<<"number of inputs "<<innames.size()<<endl;
  for(unsigned int i=1;i<=innames.size();i++){
    cout<<i<<" "<<innames[i-1]<<endl;
    read_volume(tmp,innames[i-1]);
    tmpvec[i-1]=tmp;
  }
  cerr<<"threshold "<<thresh<<endl;

  volume<float> total;
  volume<float> total_thresh;
  total.reinitialize(tmp.xsize(),tmp.ysize(),tmp.zsize());
  total_thresh.reinitialize(tmp.xsize(),tmp.ysize(),tmp.zsize());
  copybasicproperties(tmp,total_thresh);
  copybasicproperties(tmp,total);
  total=total*0;
  for(unsigned int i=0;i<tmpvec.size();i++){
    total+=tmpvec[i];
  }
  
  total_thresh=binarise(total,thresh);
  total.setDisplayMaximumMinimum(total.max(),total.min());
  save_volume(total,"total");
  for(unsigned int i=0;i<tmpvec.size();i++){
    tmp=divide(tmpvec[i],total,total_thresh);
    string outname =innames[i];
    make_basename(outname);
    string thrname="_thr_"+num2str(thresh);
    total.setDisplayMaximumMinimum(1,0);
    save_volume(tmp,outname+"_proj_seg"+thrname);
  }
}
void proj_thresh_surfaces(const vector<string>& innames,const float& thresh){
  cerr<<"Surface option not implemented yet"<<endl;
  exit(1);
}
bool test_input(const vector<string>& filenames){
  int nsurfs=0,nvols=0;
  for(unsigned int i=0;i<filenames.size();i++){
    if(meshExists(filenames[i])){nsurfs++;}
    else if(fsl_imageexists(filenames[i])){nvols++;}
    else{
      cerr<<"Cannot open "<<filenames[i]<<" for reading"<<endl;
      exit(1);
    }
  }
  if(nvols>0 && nsurfs>0){
    cerr<<"Please use list of EITHER volumes OR surfaces"<<endl;
    exit(1);
  }
  return (nvols>0?true:false);
}
int main ( int argc, char **argv ){
  if(argc<3){
    cerr<<""<<endl;
    cerr<<"usage:proj_thresh <lots of volumes/surfaces> threshold"<<endl;
    cerr<<"  please use EITHER volumes OR surfaces"<<endl;
    cerr<<""<<endl;
    exit(1);
  }

  vector<string> innames;
  innames.clear();
  for(int i=1;i<=argc-2;i++){
    innames.push_back(argv[i]);
  }
  bool isvols=test_input(innames);

  float thresh=atof(argv[argc-1]);
  if(!isvols){
    proj_thresh_surfaces(innames,thresh);
  }
  else{
    proj_thresh_volumes(innames,thresh);
  }

  
}










