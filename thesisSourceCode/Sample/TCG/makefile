#Project: Clo_Red_Graph_Based Floorplanning
#Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
#Author:  Jai_Ming Lin	 <gis87808@cis.nctu.edu.tw>
#Date:    7/20/2000 

.SUFFIXES: .cc 
SHELL=/bin/sh
LEDAROOT=/home2/gis878/gis87808/LEDA
TIMESTAMP=\"`date +%m/%d/%y_%H:%M:%S`\" 
STLLEDA=-DLEDA_PREFIX
FASTLEDA=-DLEDA_STL_ITERATORS
CXX=g++
CXXFLAGS= -c -g -O2 $(STLLEDA) $(FASTLEDA) -DTIMESTAMP=$(TIMESTAMP) -I$(LEDAROOT)/incl 
LDFLAGS= 
###########################################################################
LIBS = -lW -lP -lG -lL -lX11 -lm -L$(LEDAROOT)
OBJS = main.o hv_matrix.o fplan.o clo_red.o top_sort.o save_out.o 
SRCS = ${OBJS:%.o=%.cc}
EXE  = TCG

$(EXE): $(OBJS)
	  $(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)

$(OBJS): hv_matrix.h fplan.h clo_red.h top_sort.h save_out.h

clean: 
	rm -f $(OBJS); rm $(EXE); rm *~

compact : $(EXE)
	strip $?
