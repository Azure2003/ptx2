# This is the Makefile for the ptx2 project
#
# Some commands provided by ptx2 are CUDA-capable - these
# commands can be compiled by setting the "cuda" variable
# when calling make, i.e.:
#
#     make gpu=1
include $(FSLCONFDIR)/default.mk

PROJNAME = ptx2
XFILES   = probtrackx2 surfmaths surf2surf surf2volume \
           surf_proj label2surf find_the_biggest proj_thresh \
           fdt_matrix_merge

ifdef gpu
    XFILES += probtrackx2_gpu
endif

CUDALIBS = -lcudadevrt -lcudart -lcuda -lnvToolsExt
LIBS     = -lfsl-newmeshclass -lfsl-warpfns -lfsl-basisfield \
           -lfsl-surface -lfsl-vtkio -lfsl-meshclass -lfsl-newimage \
           -lfsl-utils -lfsl-miscmaths -lfsl-NewNifti -lfsl-giftiio \
           -lfsl-first_lib -lfsl-znz -lfsl-cprob -lfsl-utils -lexpat


all: ${XFILES}

PTX2OBJS = probtrackx.o probtrackxOptions.o streamlines.o ptx_simple.o \
           ptx_seedmask.o ptx_nmasks.o csv.o csv_mesh.o

probtrackx2: $(PTX2OBJS)
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

find_the_biggest: find_the_biggest.o csv_mesh.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

proj_thresh: proj_thresh.o csv_mesh.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

fdt_matrix_ops: fdt_matrix_ops.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

fdt_matrix_split: fdt_matrix_split.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

testfile: testfile.o streamlines.o csv.o csv_mesh.o probtrackxOptions.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

surf_proj: surf_proj.o csv.o csv_mesh.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

fdt_matrix_merge: fdt_matrix_merge.o streamlines.o csv.o csv_mesh.o probtrackxOptions.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

fdt_matrix_4_to_2: fdt_matrix_4_to_2.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

surf2surf: surf2surf.o csv.o csv_mesh.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

surf2volume: surf2volume.o csv.o csv_mesh.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

label2surf: label2surf.o csv_mesh.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

surfmaths: surfmaths.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

PTX2GPUOBJS = probtrackx_gpu.o tractography_gpu.o link_gpu.o \
              saveResults_ptxGPU.o CUDA/tractographyInput.o \
              CUDA/tractographyData.o probtrackxOptions.o csv.o csv_mesh.o

probtrackx2_gpu: ${PTX2GPUOBJS}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS} ${NVCCLDFLAGS}

link_gpu.o:	tractography_gpu.o
	$(NVCC) ${NVCCFLAGS} -o $@ -dlink tractography_gpu.o

tractography_gpu.o:
	$(NVCC) ${NVCCFLAGS} -dc -o $@ CUDA/tractography_gpu.cu
