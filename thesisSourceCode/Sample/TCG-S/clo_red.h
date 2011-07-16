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
class Snode{
   public:
     int label;
     int pt;

     Snode() {};
     ~Snode() {};
     Snode(const Snode& sn){
        label=sn.label;
        pt=sn.pt;
     } 
     Snode& operator=(const Snode& sn){
        label=sn.label;
        pt=sn.pt;
        return *this;
     }
     void assign_label(int l){
          label=l;
     }
     friend istream& operator>>(istream& is, Snode& sn){
        is>>sn.label>>sn.pt;
        return is;
     }   
     friend ostream& operator<<(ostream& os, const Snode& sn){
         os<<"label="<<sn.label<<", pt="<<sn.pt;
         return os;
     }          
};

struct Solution{
    //GRAPH<int, int> h_clo_G;
    //GRAPH<int, int> v_clo_G;
    Hv_Matrix triangle_matrix;
    Modules_Info modules_info;
    leda_list<Snode> packing_seq;
    int Area;
    int Wire; 
    int Width;
    int Height;
    float Cost;
};
      
class Clo_Red_Graph: public FloorPlan{
  public:
    Clo_Red_Graph() {};
    ~Clo_Red_Graph() {};
    void init();
    void init_sqr();
    void compute_cost(float, float, int, int);
    void compute_wire_length();    
    void contour_packing(); 
    
    //simulated Annealing
    void perturbate0();
    void perturbate1();
    void perturbate2();
    void perturbate3();
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
    int Pick_Red_Edge(char, leda_edge&, int&, int&, GRAPH<int, int>&);
    void Tune_Closure_Graphs(GRAPH<int, int>&, GRAPH<int, int>&,
                             leda_node, leda_node, char);
    void reconstruct(leda_list<Snode>&, leda_list<Snode>&,
                     vector<list_item>&,
                     GRAPH<int, int>&,  GRAPH<int, int>&,
                     vector<leda_node>&, vector<leda_node>&,
                     Hv_Matrix&, Hv_Matrix&);   

    Hv_Matrix triangle_matrix;
    GRAPH<int, int> h_clo_G;
    GRAPH<int, int> v_clo_G;
    vector<leda_node> h_nodes;
    vector<leda_node> v_nodes;
    leda_list<Snode> packing_seq;
    vector<list_item> packing_litems;
};
//float rand_01();
//---------------------------------------------------------------------------
#endif