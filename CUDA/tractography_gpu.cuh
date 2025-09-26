/*  tractography_gpu.cuh

    Moises Hernandez-Fernandez  - FMRIB Image Analysis Group

    Copyright (C) 2015 University of Oxford  */

/*  CCOPYRIGHT  */

#include <vector>

#include "vector_types.h"

#include "CUDA/tractographyData.h"
#include "newimage/newimageall.h"
#include "../sparse.h"



void tractography_gpu(tractographyData&                 data_host,
                      NEWIMAGE::volume<float>*&          m_prob,
                      NEWIMAGE::volume<float>*&          m_prob2,
                      int*&                              keeptotal,
                      float**                            ConNet,
                      float**                            ConNetb,
                      SparseMatrix<float>*                            ConMat1,
                      SparseMatrix<float>*                           ConMat1b,
                      SparseMatrix<float>*                           ConMat3,
                      SparseMatrix<float>*                            ConMat3b,
                      SparseMatrix<int64_t>*     ConMat4,
                      float*&                            s2targets,
                      float*&                            s2targetsb,
                      std::vector< std::vector<float> >& m_save_paths,
                    NEWIMAGE::volume<int>& lookup4,
                      NEWIMAGE::volume4D<float>*&        m_localdir);

bool compare_Vertices(const float3 &a, const float3 &b);

void make_unique(std::vector< float3 >&conns);

void write_mask3(
            unsigned long long nstreamlines,
            float3*             mat_crossed_host,
            int*                mat_numcrossed_host,
            int                 max_per_jump_mat,
            float3*             lrmat_crossed_host,
            int*                lrmat_numcrossed_host,
            int                 max_per_jump_lrmat,
            // Output
            SparseMatrix<float>*            ConMat3,
            SparseMatrix<float>*             ConMat3b);

void write_mask1(tractographyData&  data_host,
                 long long          offset_SLs,
                 unsigned long long nstreamlines,
                 float3*            lrmat_crossed_host,
                 int*               lrmat_numcrossed_host,
                 int                max_per_jump_lrmat,
                 // Output
                 SparseMatrix<float>*            ConMat1,
                 SparseMatrix<float>*           ConMat1b);

void write_Matrix4(
  tractographyData&	data_host,
  sampleResult**  sampled_fib_indices_host,
  NEWIMAGE::volume<int>& lookup4,
  long long 		offset_SLs,
  unsigned long long 	nstreamlines,
  float3*			mat_crossed_host,
  int*        mat_numcrossed_host,
  int			            max_per_jump_mat,
  SparseMatrix<int64_t>*     ConMat4,
  bool        opts_Value,
  float          steplength,
  int** lengths_host,
  float** path_host
);

void update_s2targets(    // Input
            tractographyData&         data_host,
            long long                 offset_SLs,
            unsigned long long        nstreamlines,
            float**                   targvalues_host,
            float**                   targvaluesb_host,
            // Output
            NEWIMAGE::volume<float>*& m_s2targets);
