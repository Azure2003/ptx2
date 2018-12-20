/*  saveResults_ptxGPU.h

    Moises Hernandez-Fernandez  - FMRIB Image Analysis Group

    Copyright (C) 2015 University of Oxford  */

/*  CCOPYRIGHT  */

#include "CUDA/tractographyData.h"

void counter_save_total(int*& keeptotal, int numKeeptotals);

void counter_save_pathdist(volume<float>& m_prob,volume<float>& m_prob2);

void counter_save(
	tractographyData& 	data_host,
	volume<float> 		*m_prob, 	// spatial histogram of tract location within brain mask (in seed space)
	volume<float> 		*m_prob2,	// omeanpathlength
	float**			ConNet,		// Network mode
	float**			ConNetb,	// Network mode
	int 			nRowsNet,	// Network mode
	int 			nColsNet,	// Network mode
	float**			ConMat1,
	float**			ConMat1b,	// omeanpathlength
	int 			nRowsMat1,
	int 			nColsMat1,
	float**			ConMat3,
	float**			ConMat3b,
	int 			nRowsMat3,
	int 			nColsMat3,
	float*			m_s2targets,
	float*			m_s2targetsb,
	vector< vector<float> >& m_save_paths,
	volume4D<float> 	*m_localdir);

