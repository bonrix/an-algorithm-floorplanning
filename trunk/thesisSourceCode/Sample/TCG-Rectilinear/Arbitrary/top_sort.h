//---------------------------------------------------------------------------
#ifndef topH
#define topH
//---------------------------------------------------------------------------
#include <LEDA/graph_alg.h>
#include <LEDA/node_array.h>
#include <LEDA/list.h>
#include <LEDA/array.h>
#include <vector>
#include "fplan.h"
#include "clo_red.h"
//---------------------------------------------------------------------------
void Topologic_Sort(GRAPH<int, float>&, leda_node_array<int>&);
void Packing_Topologic_Sort(GRAPH<int, float> &, Modules &, leda_node_array<int>&, vector<leda_node>&, char, Modules_Info&, Hv_Matrix&);
//---------------------------------------------------------------------------
#endif






