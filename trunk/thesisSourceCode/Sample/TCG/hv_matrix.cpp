#include "hv_matrix.h"
//--------------------------------------------------------
//update graph "G" when delete edge "te" from graph
void change_graph_as_del_edge(leda_edge& te, GRAPH<int, int>& G)
{
     leda_node source_node=G.source(te);
     leda_node target_node=G.target(te);
     leda_edge e;
             
     //connect source node with sink if the node's outdegree ==1
     if(G.outdeg(source_node)==1){
         e=G.new_edge(source_node, G.last_node());
         G.assign(e, G[te]);
     }  

     //connect target node with source if the node's indegree ==1              
     if(G.indeg(target_node)==1){
         e=G.new_edge(G.first_node(), target_node);
         G.assign(e, 0);
     } 

     //delete original edge
     G.del_edge(te);
}

void error_message(char* m)
{
   cout<<m<<endl;
   exit(0);
}

//----------------------------------------------------------------

Hv_Matrix::Hv_Matrix(const Hv_Matrix& m)
{
   triangle_matrix=m.triangle_matrix;
}

Hv_Matrix& Hv_Matrix::operator=(const Hv_Matrix& m)
{
   triangle_matrix=m.triangle_matrix;

   return *this;
}

leda_edge& Hv_Matrix::get_edge(int i, int j)
{
   int module_size=triangle_matrix.size();

   if(i < j)
      return triangle_matrix[i-1][module_size-j].ed;
   else if(j < i)
      return triangle_matrix[j-1][module_size-i].ed;       
   else
   {
      cout<<"Error!!!get_edge in hv_matrix.cc"<<endl;
      exit(0);
   }
}

Edge_Type Hv_Matrix::get_edge_type(int i, int j) 
{
   int module_size=triangle_matrix.size();

   if(i<j)
      return triangle_matrix[i-1][module_size-j].direction;
   else
      return triangle_matrix[j-1][module_size-i].direction;       
}

char Hv_Matrix::get_edge_type_c(int i, int j) 
{
   int module_size=triangle_matrix.size();

   if(i<j){
       if(triangle_matrix[i-1][module_size-j].direction==H||
          triangle_matrix[i-1][module_size-j].direction==h)
          return 'H';
       else
          return 'V';
   }else{
       if(triangle_matrix[j-1][module_size-i].direction==H||
          triangle_matrix[j-1][module_size-i].direction==h)
          return 'H';
       else
         return 'V'; 
   }      
}

void Hv_Matrix::rev_edge_type(int i, int j, char d)
{
   int module_size=triangle_matrix.size();
   
   if(d=='H')
   {
      if(i<j)
        triangle_matrix[i-1][module_size-j].direction=H;
      else
        triangle_matrix[j-1][module_size-i].direction=h;
   }
   else
   {
      if(i<j)
        triangle_matrix[i-1][module_size-j].direction=V;
      else
        triangle_matrix[j-1][module_size-i].direction=v;
   }
}

void Hv_Matrix::Update_Matrix(int i, int j, char x, GRAPH<int, 
                              int>& G, leda_edge& e)
{
   int module_size=triangle_matrix.size();

   if(i < j)
   { 
      if(x=='H')
      {   
          if(triangle_matrix[i-1][module_size-j].direction!=H){
            triangle_matrix[i-1][module_size-j].direction=H;
            change_graph_as_del_edge(triangle_matrix[i-1][module_size-j].ed, G);
            triangle_matrix[i-1][module_size-j].ed=e;
          }
          else{
            cout<<"update matrix error of h edge <"<<i+1<<"," 
                <<j+1<<">"<<endl;
            exit(0);
          }
      }
      else if(x=='V')
      {   
          if(triangle_matrix[i-1][module_size-j].direction!=V)
          {
            triangle_matrix[i-1][module_size-j].direction=V;
            change_graph_as_del_edge(triangle_matrix[i-1][module_size-j].ed, G);
            triangle_matrix[i-1][module_size-j].ed=e;
          }
          else{
            cout<<"update matrix error of v edge <"<<i+1<<"," 
                <<j+1<<">"<<endl;
            exit(0);
          }
      }
      else
         error_message("update matrix error (not H or V)!!!");        
   }
   else
   {
      if(x=='H')
      {
          if(triangle_matrix[j-1][module_size-i].direction!=h)
          {
            triangle_matrix[j-1][module_size-i].direction=h; 
            change_graph_as_del_edge(triangle_matrix[j-1][module_size-i].ed, G);
            triangle_matrix[j-1][module_size-i].ed=e;
          }
          else{
            cout<<"update matrix error of h edge <"<<j+1<<"," 
                <<i+1<<">"<<endl;
            exit(0);
          }
      }
      else if(x=='V')
      {
          if(triangle_matrix[j-1][module_size-i].direction!=v)
          {
            triangle_matrix[j-1][module_size-i].direction=v; 
            change_graph_as_del_edge(triangle_matrix[j-1][module_size-i].ed, G);
            triangle_matrix[j-1][module_size-i].ed=e;
          }
          else{
            cout<<"update matrix error of v edge <"<<j+1<<"," 
                <<i+1<<">"<<endl;
            exit(0);
          }
      }
      else
         error_message("update matrix error (not H or V)!!!");        
   }
}

void Hv_Matrix::set_edge(int i, int j, leda_edge& e)
{
   int module_size=triangle_matrix.size();

   if(i < j) 
     triangle_matrix[i-1][module_size-j].ed= e;
   else if(i > j)
     triangle_matrix[j-1][module_size-i].ed= e;
   else
   {
      cout<<"Error!!!set_edge in hv_matrix.cc."<<endl;
      exit(0);
   }  
}

void Hv_Matrix::set_edge_type(int i, int j, Edge_Type et)
{
    int module_size=triangle_matrix.size();
    
    if(i < j)
      triangle_matrix[i-1][module_size-j].direction= et;
    else if(j < i)
      triangle_matrix[j-1][module_size-i].direction= et;
    else 
    {
      cout<<"Error!!!set_edge_type in hv_matrix.cc"<<endl;
      exit(0);
    }
}

void Hv_Matrix::set_edge_type(int i, int j, char d)
{
    int module_size=triangle_matrix.size();
    
    if(i < j)
    {
      if(d=='H')
        triangle_matrix[i-1][module_size-j].direction=H;
      else
        triangle_matrix[i-1][module_size-j].direction=V;
    }
    else if(j < i)
    {
      if(d=='H')
        triangle_matrix[j-1][module_size-i].direction=h;
      else
        triangle_matrix[j-1][module_size-i].direction=v;
    }
    else 
    {
      cout<<"Error!!!set_edge_type in hv_matrix.cc"<<endl;
      exit(0);
    }
}

void Hv_Matrix::init(int module_size)
{
   int i;
  
   triangle_matrix.resize(module_size);

   for(i=0; i<module_size; i++)
           triangle_matrix[i].resize(module_size-i-1);
}

int Hv_Matrix::equal_type(int source_label, int target_label, char d)
{
   int module_size=triangle_matrix.size();

   if(d=='H'){
     if(source_label<target_label){
       if(triangle_matrix[source_label-1][module_size-target_label].direction
          ==H)
        return 1;
       else
        return 0; 
     }else{
       if(triangle_matrix[target_label-1][module_size-source_label].direction
         ==h)
        return 1;
       else
        return 0; 
     }
   }else{
     if(source_label<target_label){
       if(triangle_matrix[source_label-1][module_size-target_label].direction
          ==V)
        return 1;
       else
        return 0; 
     }else{
       if(triangle_matrix[target_label-1][module_size-source_label].direction
         ==v)
        return 1;
       else
        return 0; 
     }
   }
}

Hv_Matrix::Hv_Matrix()
{
}

Hv_Matrix::~Hv_Matrix()
{
   triangle_matrix.clear();
}

ostream& operator<<(ostream& os, const Hv_Matrix& m)
{
  int i, j;
  int module_size=m.triangle_matrix.size();

  for(i=0; i< module_size; i++)
  {
     for(j=i+1; j < module_size; j++)
       os << "<" << i+2 << ", " << j+2 << ">="
          << m.triangle_matrix[i][module_size-j-1].direction<<endl; 
  }  
  return os;
}