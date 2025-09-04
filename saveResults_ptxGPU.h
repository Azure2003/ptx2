/*  saveResults_ptxGPU.h

    Moises Hernandez-Fernandez  - FMRIB Image Analysis Group

    Copyright (C) 2015 University of Oxford  */

/*  CCOPYRIGHT  */

#include <vector>

#include "newimage/newimage.h"

#include "CUDA/tractographyData.h"
#include "sparse.h"


void counter_save_total(int*& keeptotal, int numKeeptotals);

void counter_save_pathdist(NEWIMAGE::volume<float>& m_prob,NEWIMAGE::volume<float>& m_prob2);

void counter_save(
	tractographyData& 	data_host,
	NEWIMAGE::volume<float> 		*m_prob, 	// spatial histogram of tract location within brain mask (in seed space)
	NEWIMAGE::volume<float> 		*m_prob2,	// omeanpathlength
	float**			ConNet,		// Network mode
	float**			ConNetb,	// Network mode
	int 			nRowsNet,	// Network mode
	int 			nColsNet,	// Network mode
	SparseMatrix<float>*			ConMat1,
	SparseMatrix<float>*			ConMat1b,	// omeanpathlength
	int 			nRowsMat1,
	int 			nColsMat1,
	SparseMatrix<float>*			ConMat3,
	SparseMatrix<float>*			ConMat3b,
	int 			nRowsMat3,
	int 			nColsMat3,
	SparseMatrix<int64_t>*			ConMat4,
	int 			nRowsMat4,
	int 			nColsMat4,
	float*			m_s2targets,
	float*			m_s2targetsb,
	std::vector< std::vector<float> >& m_save_paths,
	NEWIMAGE::volume4D<float> 	*m_localdir);
