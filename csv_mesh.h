#if !defined (CSV_MESH_H)
#define CSV_MESH_H


#define CSV_ASCII 5
#define CSV_VTK   6
#define CSV_GIFTI 7

#include <stdio.h>
#include "meshclass/point.h"
#include "meshclass/mpoint.h"
#include "miscmaths/miscmaths.h"
#include "fslsurface/fslsurfaceio.h"
#include "fslsurface/fslsurface.h"

using namespace fslsurface_name;
using namespace std;
using namespace MISCMATHS;
using namespace mesh;

int  meshFileType(const string& filename);
bool meshExists(const string& filename);


class CsvMpoint {

 public:

  CsvMpoint(double x, double y, double z, int counter):_no(counter){
    _coord=Pt(x, y, z);
  }
  CsvMpoint(const Pt p, int counter,float val=0):_no(counter){
    _coord=p;
  }
  ~CsvMpoint(){}
  CsvMpoint(const CsvMpoint &p):_coord(p._coord),_no(p._no),_trID(p._trID){
    *this=p;
  }
  CsvMpoint& operator=(const CsvMpoint& p){
    _coord=p._coord;
    _no=p._no;
    _trID=p._trID;
    return(*this);
  }

  const Pt&    get_coord() const         {return _coord;}
  void         set_coord(const Pt& coord){_coord=coord;}
  const int&   get_no() const            {return _no;}
  void         push_triangle(const int& i){_trID.push_back(i);}
  int          ntriangles()const{return (int)_trID.size();}
  int          get_trID(const int i)const{return _trID[i];}

 private:
  Pt                  _coord;
  int                 _no;
  vector<int>         _trID;
};

class CsvTriangle {
 private:
  vector<CsvMpoint> _vertice;
  int               _no;

 public:
  CsvTriangle(){}
  CsvTriangle(const CsvMpoint& p1,const CsvMpoint& p2,const CsvMpoint& p3,int no):_no(no){
    _vertice.push_back(p1);
    _vertice.push_back(p2);
    _vertice.push_back(p3);
  }
  ~CsvTriangle(){}
  CsvTriangle(const CsvTriangle &t):_vertice(t._vertice),_no(t._no){
    *this=t;
  }
  CsvTriangle& operator=(const CsvTriangle& t){
    _vertice=t._vertice;_no=t._no;return *this;
  }

  Vec  centroid() const{
    Vec p ((_vertice[0].get_coord().X +_vertice[1].get_coord().X +_vertice[2].get_coord().X)/3,
	  (_vertice[0].get_coord().Y +_vertice[1].get_coord().Y +_vertice[2].get_coord().Y)/3,
	  (_vertice[0].get_coord().Z +_vertice[1].get_coord().Z +_vertice[2].get_coord().Z)/3);

    return p;
  }
  Vec normal()const{
    Vec result=(_vertice[2].get_coord()-_vertice[0].get_coord())*(_vertice[1].get_coord()-_vertice[0].get_coord());
    result.normalize();
    return result;     
  }
  bool isinside(const Vec& x)const;
  double dist_to_point(const Vec& x0)const;


  const CsvMpoint& get_vertice(const int& i) const{return _vertice[i];}
  const bool intersect(const vector<Pt> & p)const;          // checks if a segment intersects the triangle
  const bool intersect(const vector<Pt> & p,int& ind)const; // checks if a segment intersects the triangle+gives the index of the closest vertex
  int get_no()const{return _no;}
};


const bool operator ==(const CsvMpoint &p2, const CsvMpoint &p1);
const bool operator ==(const CsvMpoint &p2, const Pt &p1);

const Vec operator -(const CsvMpoint&p1, const CsvMpoint &p2);
const Vec operator -(const Pt&p1, const CsvMpoint &p2);
const Vec operator -(const CsvMpoint&p1, const Pt &p2);
const Vec operator -(const ColumnVector& p1, const CsvMpoint &p2);
const Vec operator -(const ColumnVector& p1, const Vec &p2);
const Vec operator -(const Vec& p1, const CsvMpoint &p2);


class CsvMesh {

 public:
  vector<CsvMpoint>   _points;
  vector<CsvTriangle> _triangles;
  vector<float>       _pvalues;
  vector<float>       _tvalues;
  

  CsvMesh(){}
  ~CsvMesh(){}
  CsvMesh(const CsvMesh &m):_points(m._points),_triangles(m._triangles),_pvalues(m._pvalues),_tvalues(m._tvalues){
    *this=m;
  }
  CsvMesh& operator=(const CsvMesh& m){
    _points=m._points;
    _triangles=m._triangles;
    _pvalues=m._pvalues;
    _tvalues=m._tvalues;
    return(*this);
  }
  
  void        clear(){
    _points.clear();_triangles.clear();_pvalues.clear();_tvalues.clear();
  }

  int   nvertices() const{return (int)_points.size();}
  int   ntriangles() const{return (int)_triangles.size();}
  const CsvMpoint&    get_point(int n)const{return _points[n];}
  const CsvTriangle&  get_triangle(int n)const{return _triangles[n];}

  float get_pvalue(const int& i)const{return _pvalues[i];}
  float get_tvalue(const int& i)const{return _tvalues[i];}
  void set_pvalue(const int& i,const float& val){_pvalues[i]=val;}
  void set_tvalue(const int& i,const float& val){_tvalues[i]=val;}
  void set_pvalues(const vector<int>& ids,const float& val){
    for(unsigned int i=0;i<ids.size();i++){_pvalues[ids[i]]=val;}
  }
  void set_pvalues(const ColumnVector& vals){
    if(vals.Nrows()!=(int)_points.size()){
      cerr<<"CsvMesh::set_pvalues:incompatible number of scalars"<<endl;
      exit(1);
    }
    for(int i=1;i<=vals.Nrows();i++)
      _pvalues[i-1]=vals(i);
  }

  vector< float > getPointsAsVectors()const{
    vector< float > ret;
    for(int i=0;i<nvertices();i++){
      ret.push_back( get_point(i).get_coord().X );
      ret.push_back( get_point(i).get_coord().Y );
      ret.push_back( get_point(i).get_coord().Z );
    }
    return ret;
  }
  vector< float > getValuesAsVectors()const{
    vector< float > ret;
    for(int i=0;i<nvertices();i++){
      ret.push_back( get_pvalue(i) );
    }
    return ret;
  }
  vector< vector<unsigned int> > getTrianglesAsVectors()const{
    vector< vector<unsigned int> > ret;
    for(int i=0;i<ntriangles();i++){
      vector<unsigned int> tmp(3);
      tmp[0]=(unsigned int)_triangles[i].get_vertice(0).get_no();
      tmp[1]=(unsigned int)_triangles[i].get_vertice(1).get_no();
      tmp[2]=(unsigned int)_triangles[i].get_vertice(2).get_no();
      ret.push_back(tmp);
    }
    return ret;
  }

  void reset_pvalues(){
    _pvalues.clear();
    for(unsigned int i=0;i<_points.size();i++)
      _pvalues.push_back(0);
  }
  void reset_tvalues(){
    _tvalues.clear();
    for(unsigned int i=0;i<_triangles.size();i++)
      _tvalues.push_back(0);  
  }

  void push_triangle(const CsvTriangle& t);
  Vec  local_normal(const int& pt)const{
    Vec v(0,0,0);
    for(int i=0;i<_points[pt].ntriangles();i++){      
      v+=_triangles[ _points[pt].get_trID(i) ].normal();
    }
    v.normalize();
    return v;    
  }

  int step_sign(const int& vertind,const Vec& step)const;

  void load(const string& filename); 
  void load_gifti(const string& filename);
  void load_ascii(const string& filename);
  void load_vtk(const string& filename);

  void save(const string& filename, const int& type=GIFTI_ENCODING_B64GZ)const;
  void save_ascii(const string& s)const;
  void save_gifti(const string& s,const int& type=GIFTI_ENCODING_B64GZ)const;
  void save_vtk(const string& s)const;


  void print(const string& filename){
    ofstream fs(filename.c_str());
    fs<<_points.size()<<" vertices"<<endl;
    for(unsigned int i=0;i<_points.size();i++){
      fs<<_points[i].get_coord().X<<" "
	  <<_points[i].get_coord().Y<<" "
	  <<_points[i].get_coord().Z<<endl;
    }
    fs<<_triangles.size()<<" triangles"<<endl;
    for(unsigned int i=0;i<_triangles.size();i++){
      fs<<_triangles[i].get_vertice(0).get_no()<<" "
	  <<_triangles[i].get_vertice(1).get_no()<<" "
	  <<_triangles[i].get_vertice(2).get_no()<<endl;
    }
    fs.close();
  }

  //  ostream& operator <<(ostream& flot);
};



#endif
