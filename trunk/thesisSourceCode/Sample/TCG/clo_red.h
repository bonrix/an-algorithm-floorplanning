//---------------------------------------------------------------------------
#ifndef clo_redH
#define clo_redH
//---------------------------------------------------------------------------
#define DEBUG_UT 1

#include <LEDA/graph.h>
#include <LEDA/node_array.h>
#include "hv_matrix.h"
#include "fplan.h"
//--------------------------------------------------------------------------
struct Solution{
    GRAPH<int, int> h_clo_G;
    GRAPH<int, int> v_clo_G;
    Modules_Info modules_info;
    int Area;
    int Wire; 
    int Width;
    int Height;
    double Cost;
};
      

class Clo_Red_Graph: public FloorPlan{
  public:
    Clo_Red_Graph() {};
    ~Clo_Red_Graph() {};
    void init();
    void init_square();
    void Compute_Cost(char, double, double, int&, int&);
    void Compute_Wire_Length();    
    void packing();
    
    //simulated Annealing
    void permutate1();
    void permutate2();
    void permutate3();
    float get_cost();
    void get_solution(Solution&);
    void recover(Solution &sol);
   
    //debug tools
    void Test();
    void list_information();  
    
  private:
    //simulated operators
    int reverse_edge(GRAPH<int, int>&, GRAPH<int, int>&, char);
    int move_edge(GRAPH<int, int>&, GRAPH<int, int>&, char);
    int move_edge_reverse(GRAPH<int, int>&, GRAPH<int, int>&, char);
    void rotate(int);
    void interchange();
    int Pick_Red_Edge(GRAPH<int, int>&, leda_edge&);
    void Tune_Closure_Graphs(GRAPH<int, int>&, GRAPH<int, int>&,
                             leda_node, leda_node, char);
   
    Hv_Matrix triangle_matrix;
    GRAPH<int, int> h_clo_G;
    GRAPH<int, int> v_clo_G;
};
//float rand_01();
//---------------------------------------------------------------------------
#endif