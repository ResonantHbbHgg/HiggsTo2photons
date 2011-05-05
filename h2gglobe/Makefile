# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

include Makefile.arch

SrcSuf        = cc

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:    

LOOPALL = LoopAll

LOOPALLSO = libLoopAll.$(DllSuf)

# integrate VertexAnalysis sub-package
VTX=VertexAnalysis
VTXSRC=$(wildcard $(VTX)/src/*.$(SrcSuf))
VTXOBS=$(patsubst %$(SrcSuf), %$(ObjSuf), $(VTXSRC))

PHO=PhotonAnalysis
PHOSRC=$(wildcard $(PHO)/src/*.$(SrcSuf))
PHOOBS=$(patsubst %$(SrcSuf), %$(ObjSuf), $(PHOSRC))

LOOPALLO = LoopAll.$(ObjSuf) \
	   LoopAllDict.$(ObjSuf) \
	   dict.$(ObjSuf) \
	   HistoContainer.o \
	   BaseAnalysis.o \
	   CounterContainer.o \
	   SampleContainer.o \
	   Cut.o \
           $(VTXOBS) $(PHOOBS)

all: $(LOOPALL)

clean:
	@rm -f $(LOOPALLO) core *Dict.* *.so

.SUFFIXES: .$(SrcSuf)

CXXFLAGS+=-I$(shell pwd)

$(LOOPALL):  $(LOOPALLO)
	$(LD) $(SOFLAGS) $(LDFLAGS) $(ROOTLIBS)  $(LOOPALLO) $(OutPutOpt) $(LOOPALLSO)
	@echo "$(LOOPALLSO) done"

LoopAll.$(ObjSuf): CommonParameters.h LoopAll.h Tools.h \
	branchdef/Limits.h branchdef/treedef.h branchdef/newclonesarray.h \
	branchdef/treebranch.h branchdef/setbranchaddress.h branchdef/getentry.h branchdef/getbranch.h branchdef/branchdef.h \
	GeneralFunctions_cc.h GeneralFunctions_h.h \
	BaseAnalysis.cc BaseAnalysis.h \
	HistoContainer.cc HistoContainer.h \
	CounterContainer.cc CounterContainer.h \
	SampleContainer.cc SampleContainer.h \
	Cut.cc Cut.h $(VTXSRC) $(PHOSRC)

LoopAllDict.$(SrcSuf): CommonParameters.h LoopAll.h \
	branchdef/Limits.h branchdef/treedef.h \
	GeneralFunctions_h.h \
	BaseAnalysis.h \
	HistoContainer.h \
	CounterContainer.h \
	SampleContainer.h \
	Cut.h \
	VertexAnalysis/interface/VertexAlgoParameters.h PhotonAnalysis/interface/PhotonAnalysis.h

	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c LoopAll.h BaseAnalysis.h VertexAnalysis/interface/VertexAlgoParameters.h PhotonAnalysis/interface/PhotonAnalysis.h
	@rootcint -f dict.cpp -c -p LinkDef.h 

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -g -c $< -o $@

