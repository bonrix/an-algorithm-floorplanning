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
    GRAPH<int, float> h_clo_G;
    GRAPH<int, float> v_clo_G;
    Modules_Info modules_info;
    float Area;
    float Wire; 
    float Width;
    float Height;
    float Cost;
};
      

class Clo_Red_Graph: public FloorPlan{
  public:
    Clo_Red_Graph() {};
    ~Clo_Red_Graph() {};
    void init();
    void init_square();
    void Compute_Cost(float, float, Solution&);
    void Compute_Wire_Length();    
    void packing();
    
    //simulated Annealing
    void permutate();
    void permutate_1();
    void permutate_2();
    void permutate_3();
    float get_cost();
    void get_solution(Solution&);
    void recover(Solution &sol);
   
    //debug tools
    void Test();
    void list_information();  
    
  private:
    //simulated operators
    float Acyclic_Longest_Path(GRAPH<int, float>&,  char, vector<leda_node>&);
    
    int reverse_edge(GRAPH<int, float>&, GRAPH<int, float>&, char);
    
    int move_edge(GRAPH<int, float>&, GRAPH<int, float>&, char);
    
    int move_edge_reverse(GRAPH<int, float>&, GRAPH<int, float>&, char);
    
    int rotate(int);
    int Rotate_Rectilinear(GRAPH<int, float>&, GRAPH<int, float>&, int, int, char, leda_edge, leda_list<leda_node>&); 
    void Temporary_Move_Edges(GRAPH<int, float>&, int, int, char, leda_edge, leda_list<leda_node>&, leda_list<leda_edge>&);
    void Recover_Move_Edges(GRAPH<int, float>&, leda_list<leda_edge>&, char);  
    void Move_Edges(GRAPH<int, float>&, GRAPH<int, float>&, int, int, char, leda_edge, leda_list<leda_node>&);     
    
    void interchange();
    void Rotate_Submodules(int, int);
    
    void Flipping_Rectilinear(char);
    void Interchange_Rectilinear(int, int);
    
    int Pick_Red_Edge(GRAPH<int, float>&, leda_edge&, leda_node&);
    int Compute_Move_Edges(GRAPH<int, float>&, GRAPH<int, float>&, leda_node, leda_node, leda_list<leda_node>&, char);  
    void Tune_Closure_Graphs(GRAPH<int, float>&, GRAPH<int, float>&, leda_node, leda_node, char);

    void Debug();                
    void Revese_relation(int);
    vector<leda_node> h_nodes;
    vector<leda_node> v_nodes;   
    Hv_Matrix triangle_matrix;
    GRAPH<int, float> h_clo_G;
    GRAPH<int, float> v_clo_G;
};
//float rand_01();
//---------------------------------------------------------------------------
#endif