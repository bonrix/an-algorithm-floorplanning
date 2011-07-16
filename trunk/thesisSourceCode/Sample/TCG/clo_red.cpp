#include <LEDA/array.h>
#include <LEDA/graph_iterator.h>
#include <LEDA/list.h>
#include <vector>
#include <math.h>
#include <time.h>
#include "top_sort.h"
#include "clo_red.h"
//---------------------------------------------------------------
// Initialization
//---------------------------------------------------------------
void Clo_Red_Graph::init_square()
{
   int i, j;
   leda_node n;
   leda_edge e;
   vector<leda_node> node_array_hG;
   vector<leda_node> node_array_vG;
   int module_size=modules_N;
   triangle_matrix.init(module_size);

   //construct transitive closure for horizontal (vertical) graph
   //node 0 (module_size+1) denote source (sink) node
   for(i=0; i <= module_size+1; i++)
   {
      n=h_clo_G.new_node();
      node_array_hG.push_back(n);
      h_clo_G.assign(n, i);

      n=v_clo_G.new_node();
      node_array_vG.push_back(n);
      v_clo_G.assign(n, i);
   }

   int k, l=1;
   int len=(int)ceil(sqrt(modules_N));
   for(i=1; i <modules_N; i++){
      l=((i-1)%len)+1;
      for(j=i+1; j<= modules_N; j++){
         if(l<len){
            e=h_clo_G.new_edge(node_array_hG[i], node_array_hG[j]);
            h_clo_G.assign(e, modules[i-1].width);

            triangle_matrix.set_edge(i, j, e);
            triangle_matrix.set_edge_type(i, j, H);
         }else{
            e=v_clo_G.new_edge(node_array_vG[i], node_array_vG[j]);
            v_clo_G.assign(e, modules[i-1].height);

            triangle_matrix.set_edge(i, j, e);
            triangle_matrix.set_edge_type(i, j, V);
         }
         l++;
      }
   }
 
   forall_nodes(n, h_clo_G){
      if(h_clo_G.indeg(n)==0){
         e=h_clo_G.new_edge(node_array_hG[0], 
                            node_array_hG[h_clo_G[n]]);
         h_clo_G.assign(e, 0);
      }

      if(h_clo_G.outdeg(n)==0){
         e=h_clo_G.new_edge(node_array_hG[h_clo_G[n]], 
                            node_array_hG[modules_N+1]);
         h_clo_G.assign(e, modules[h_clo_G[n]].width);
      }
   }

   forall_nodes(n, v_clo_G){
      if(v_clo_G.indeg(n)==0){
         e=v_clo_G.new_edge(node_array_vG[0], 
                            node_array_vG[v_clo_G[n]]);
         v_clo_G.assign(e, 0);
      }

      if(v_clo_G.outdeg(n)==0){
         e=v_clo_G.new_edge(node_array_vG[v_clo_G[n]], 
                            node_array_vG[modules_N+1]);
         v_clo_G.assign(e, modules[v_clo_G[n]].height);
      }
   }  
}

void Clo_Red_Graph::init()
{
   int i, j;
   leda_node n;
   leda_edge e;
   vector<leda_node> node_array_hG;
   vector<leda_node> node_array_vG;
   int module_size=modules_N;
   triangle_matrix.init(module_size);

   //construct transitive closure for horizontal (vertical) graph
   //node 0 (module_size+1) denote source (sink) node
   for(i=0; i <= module_size+1; i++)
   {
      n=h_clo_G.new_node();
      node_array_hG.push_back(n);
      h_clo_G.assign(n, i);

      n=v_clo_G.new_node();
      node_array_vG.push_back(n);
      v_clo_G.assign(n, i);
   }

   //construct edges in horizontal (vertical) transitive graphs
   for(i=0; i < module_size+1; i++)
   {
      //horizontal graph
      if((i!=0) && (i!=module_size))
      {
         for(j=i+1; j< module_size+1;j++)
         {
            e=h_clo_G.new_edge(node_array_hG[i], node_array_hG[j]);
            h_clo_G.assign(e, modules[i-1].width);

            triangle_matrix.set_edge(i, j, e);
            triangle_matrix.set_edge_type(i, j, H);
         }
      }
      else
      {
        e=h_clo_G.new_edge(node_array_hG[i], node_array_hG[i+1]);

        if(i==0)
           h_clo_G.assign(e, 0);
        else
           h_clo_G.assign(e, modules[i-1].width);
       }

      //vertical graph
      if(i!=module_size)
      {
        e=v_clo_G.new_edge(node_array_vG[0], node_array_vG[i+1]);
        v_clo_G.assign(e, 0);

        e=v_clo_G.new_edge(node_array_vG[i+1],
                               node_array_vG[module_size+1]);
        v_clo_G.assign(e, modules[i].height);
      }
   }
}
//---------------------------------------------------------------
// Placement Modules
//---------------------------------------------------------------
//compute longest path in acyclic graph
int Acyclic_Longest_Path(GRAPH<int, int>& G, 
                         Modules_Info& modules_info, 
                         Modules& modules, char d)
{
   leda_edge e;

   //acyclic graph: establish topological order
   leda_node_array<int> top_ord(G);
   Topologic_Sort(G, top_ord); //top_ord is now a topological ordering of G

   int n=G.number_of_nodes();
   leda_array<leda_node> v(1, n);
   leda_node w;
   leda_node_array<int> dist(G);
   leda_node_array<leda_edge> pred(G);

   forall_nodes(w, G)
   {
      v[top_ord[w]]=w;  //top_ord[v[i]]==i for all i
      dist[w]=-1;
      pred[w]=nil;
   }

   //relax each vertex in topological order
   leda_node s=G.first_node();
   dist[s]=0;
   for(int i=top_ord[s]; i <= n; i++)
   {
       leda_node u=v[i];
       if(pred[u]==nil && u!= s) continue;

       leda_edge e;
       int du=dist[u];
       int length;
       int label=G[u];

       forall_adj_edges(e, u)
       {
          leda_node w=G.target(e);
          length=G[e];
 
          if(pred[w]==nil || du+length > dist[w])
          {
             pred[w]=e;
             dist[w]=du+length;
          }
       }
   }

   forall_nodes(w, G){
      int label=G[w];
       
      if(label!=0&&label!=G.number_of_nodes()-1){ 
        if(d=='H'){
           modules_info[label-1].x=dist[w];
           modules_info[label-1].rx=dist[w]+modules_info[label-1].width;
        }else{
           modules_info[label-1].y=dist[w];
           modules_info[label-1].ry=dist[w]+modules_info[label-1].height;
        }
      }
   }

   return dist[v[n]];
}

void Clo_Red_Graph::packing()
{  
   Width  = Acyclic_Longest_Path(h_clo_G, modules_info, modules, 'H');
   Height = Acyclic_Longest_Path(v_clo_G, modules_info, modules, 'V');
   Area   = Height*Width; 
//   Compute_Wire_Length();

   if(Width==0)
   {
      cout<<"Error in packing for horizontal graph in clo_red.cc"<<endl;
      cout<<h_clo_G<<endl;
      exit(0);
   }
   else if(Height==0)
   {
      cout<<"Error in packing for vertical graph in clo_red.cc"<<endl;
      cout<<v_clo_G<<endl;
      exit(0);
   }
}

void Clo_Red_Graph::Compute_Wire_Length(){
  int length=0;

  // compute absolute position
  int s=modules_info.size();

  for(int i=0; i < s; i++){
    int mx= modules_info[i].x, my= modules_info[i].y;
    int width=modules[i].width;
    int height=modules[i].height;

    for(int j=0; j < modules[i].pins.size(); j++){
      Pin &p = modules[i].pins[j];
      int p_x, p_y;

      switch(modules_info[i].degree){
        case 0:{
                 p_x=p.x;
                 p_y=p.y;
                 break;
               }
        case 1:{
                 p_x=p.y;
                 p_y=width-p.x;
                 break;
               }
        case 2:{
                 p_x=width-p.x;
                 p_y=height-p.y;
                 break;
               }
        case 3:{
                 p_x=height-p.y;
                 p_y=p.x;
                 break;
               }
      }

      p.ax=p_x+mx;
      p.ay=p_y+my;
    }
  }

  float x_ratio=(float)Width/modules[s].width;
  float y_ratio=(float)Height/modules[s].height;
  for(int j=0; j < modules[s].pins.size(); j++){
      Pin &p = modules[s].pins[j];
      p.ax=int(p.x*x_ratio);
      p.ay=int(p.y*y_ratio);
  }

  for(int i=0; i < network.size(); i++){
    assert(network[i].size() > 0);
    int max_x= INT_MIN, max_y= INT_MIN;
    int min_x= INT_MAX, min_y= INT_MAX;

    for(int j=0; j < network[i].size(); j++){
      Pin& p = *network[i][j];
      max_x= max(max_x, p.ax);
      max_y= max(max_y, p.ay);
      min_x= min(min_x, p.ax);
      min_y= min(min_y, p.ay);
    }
    length += (max_x-min_x)+(max_y-min_y);
  }
  Wire=length;
}

void Clo_Red_Graph::Compute_Cost(char mode, 
     double alpha, double beta, int& A, int& W){
  
   if(mode=='s')
     Cost=alpha*Area/A+beta*Wire/W;
   else if(mode=='w')
     Cost=Wire;
   else 
     Cost=Area;
}
//---------------------------------------------------------------------------
//   Simulated Annealing Permutation Operations
//---------------------------------------------------------------------------
int Clo_Red_Graph::Pick_Red_Edge(GRAPH<int, int>& G, leda_edge& pick_e)
{
  // cout<<G<<endl;
   leda_edge e;
   //acyclic graph: establish topological order
   leda_node_array<int> top_ord(G);
   Topologic_Sort(G, top_ord); //top_ord is now a topological ordering of G
   //cout<<"start pick"<<endl;
   int n=G.number_of_nodes();
   leda_array<leda_node> v(1, n);
   leda_node w;
   leda_node_array<int> dist(G);
   leda_node_array<leda_edge> pred(G);

   forall_nodes(w, G){
      v[top_ord[w]]=w;  //top_ord[v[i]]==i for all i
      dist[w]=-1;
      pred[w]=nil;
   }

   //random choose a source and a target
   int s_index, t_index;
   int count=0;
   leda_node sn, tn;
   do{
       if(count==3)
          return 0;
       leda_edge choose_edge=G.choose_edge();
       sn=G.source(choose_edge);
       tn=G.target(choose_edge);
       count++;
   }while(G[sn]==0||G[tn]==n-1);

   s_index=top_ord[sn];
   t_index=top_ord[tn];
   
   //relax each vertex in topological order
   leda_node s=sn;
   dist[s]=0;
   leda_list<leda_node> nodes;
  
   for(int i=s_index; i <= t_index; i++){
       leda_node u=v[i];
       if(pred[u]==nil && u!= s)
         continue;
       
       nodes.append(u);
       leda_edge e;
       int du=dist[u];
       int length;
       int label=G[u];

       forall_adj_edges(e, u){
          leda_node w=G.target(e);
          if(top_ord[w] <= t_index){
             length=G[e];
             if(pred[w]==nil || du+length > dist[w]){
                pred[w]=e;
                dist[w]=du+length;
                //cout<<"dist="<<dist[w]<<endl;
              }
          }
       }
   }
   int size=nodes.size();
   int offset;
   list_item litem;

   if(size>2){
     offset=rand()%(size-1);
     litem=nodes[offset+1]; 
     pick_e = pred[nodes[litem]];
   }else
     pick_e = pred[nodes.Pop()];

   return 1;     
} 
//***************************************************************
////operator 1::move an edge from one graph to another
//***************************************************************
//Tuning the graph "trans_closure_G" when we add an edge to a 
//new graph.
void Tune_Graph_when_Add(GRAPH<int, int>& CG_to,
     leda_node& edge_source_node, leda_node& edge_target_node, 
     Modules_Info& modules_info, GRAPH<int, int>& CG_from, char d,
     Hv_Matrix& triangle_matrix)
{
   leda_edge e;
   leda_list<leda_edge> edges;
   leda_node graph_source_node=CG_to.first_node();
   leda_node graph_sink_node=CG_to.last_node();

   //case 1: If edge's target node connect to graph's source, delete
   //        the edge which connects two nodes
   //
    edges=CG_to.in_edges(edge_target_node);

    if(edges.size()==1)
    {
      e=edges.pop();
      if(CG_to.source(e)==graph_source_node)
         CG_to.del_edge(e);
    }

   //case 2: If edge's source node connect to graph's sink, delete
   //        the edge which connects two nodes
   //
    edges=CG_to.out_edges(edge_source_node);

    if(edges.size()==1)
    {
      e=edges.pop();
      if(CG_to.target(e)==graph_sink_node)
         CG_to.del_edge(e);
    }

    int edge_source_label=CG_to[edge_source_node];
    int edge_target_label=CG_to[edge_target_node];
    int edge_length;
    if(d=='H')
       edge_length=modules_info[edge_source_label-1].width;
    else
      edge_length=modules_info[edge_source_label-1].height;

    e=CG_to.new_edge(edge_source_node, edge_target_node);
    CG_to.assign(e, edge_length);
    triangle_matrix.Update_Matrix(edge_source_label, edge_target_label, d, 
                                  CG_from, e);
}

void Clo_Red_Graph::Tune_Closure_Graphs(GRAPH<int, int>& CG_to,
                                        GRAPH<int, int>& CG_from,
                                        leda_node edge_source_node,
                                        leda_node edge_target_node, char d)
{
    int edge_source_label=CG_to[edge_source_node];
    int edge_target_label=CG_to[edge_target_node];
    int edge_length;
    leda_edge e;
    int num_of_nodes=CG_to.number_of_nodes();
    
    //incremental update edges
    leda_edge se, te;
    leda_node sn, tn;
    int s_label, t_label;
    forall_in_edges(se, edge_source_node){
      sn=CG_to.source(se);
      s_label=CG_to[sn];    
      if(s_label!=0){
        if(triangle_matrix.get_edge_type_c(s_label, edge_target_label)!=d){
          if(d=='H')
            edge_length=modules_info[s_label-1].width;
          else
            edge_length=modules_info[s_label-1].height;
      
          e=CG_to.new_edge(sn, edge_target_node);
          CG_to.assign(e, edge_length);
          triangle_matrix.Update_Matrix(s_label, edge_target_label, d, CG_from, e);    
        }
      }
    }

    forall_out_edges(te, edge_target_node){
      tn=CG_to.target(te);
      t_label=CG_to[tn];    
      if(t_label!=(num_of_nodes-1)){
        if(triangle_matrix.get_edge_type_c(edge_source_label, t_label)!=d){
          //cout<<"t_label="<<t_label+1<<endl;
          if(d=='H')
            edge_length=modules_info[edge_source_label-1].width;
          else
            edge_length=modules_info[edge_source_label-1].height;
      
          e=CG_to.new_edge(edge_source_node, tn);
          CG_to.assign(e, edge_length);
          triangle_matrix.Update_Matrix(edge_source_label, t_label, d, CG_from, e);    
        }
      }
    }

    forall_in_edges(se, edge_source_node){
      sn=CG_to.source(se);
      s_label=CG_to[sn];
      forall_out_edges(te, edge_target_node){
        tn=CG_to.target(te);
        t_label=CG_to[tn];    
        if(s_label!=0&&t_label!=(num_of_nodes-1)){
          if(triangle_matrix.get_edge_type_c(s_label, t_label)!=d){
            //cout<<"s_label="<<s_label+1<<endl;
            //cout<<"t_label="<<t_label+1<<endl;
            if(d=='H')
              edge_length=modules_info[s_label-1].width;
            else
              edge_length=modules_info[s_label-1].height;
      
            e=CG_to.new_edge(sn, tn);
            CG_to.assign(e, edge_length);
            triangle_matrix.Update_Matrix(s_label, t_label, d, CG_from, e);    
          }
        }
      }
    }     
}

void Get_Nodes_of_Labels(GRAPH<int, int>& G, int source_label, 
    int target_label, leda_node& source_node, leda_node&  target_node)
{
    NodeIt it(G);
    int i;
    if(source_label<target_label)
    {
       for(i=0; i<target_label; i++)
       {
           ++it;
           if(i==source_label-1)
             source_node=it.get_node();
       }
       target_node=it.get_node();
    }
    else
    {
       for(i=0; i<source_label; i++)
       {
           ++it;
           if(i==target_label-1)
             target_node=it.get_node();
       }
       source_node=it.get_node();
    }
}

int Clo_Red_Graph::move_edge(GRAPH<int, int>& CG_from,
                             GRAPH<int, int>& CG_to, char d)
{
   leda_edge move_edge;
   leda_node source_node, target_node;
   int source_label, target_label;
   int success=1;

   success=Pick_Red_Edge(CG_from, move_edge);
   if(success){
     //extract the corresponding source (target) nodes from another
     //closure graph
     source_node=CG_from.source(move_edge);
     target_node=CG_from.target(move_edge);
     source_label=CG_from[source_node];
     target_label=CG_from[target_node];
     //cout<<"(M) change edge==>"<<"<"<<source_label+1<<", "
     //    <<target_label+1<<">"<<endl;
     Get_Nodes_of_Labels(CG_to, source_label, target_label,
                         source_node, target_node);
    
     Tune_Graph_when_Add(CG_to, source_node, target_node, 
                         modules_info, CG_from, d, triangle_matrix);
     Tune_Closure_Graphs(CG_to, CG_from, source_node, target_node, d);  
     return 1;
   }
   else
     return 0;
}

//***************************************************************
////operator 2::reverse edge direction in one graph
//***************************************************************
void Tune_Graph_when_Rev(GRAPH<int, int>& CG_invert,
                        leda_edge& invert_edge,
                        Hv_Matrix& triangle_matrix, char d, 
                        int source_label, int target_label,
                        Modules_Info& modules_info)
{
   leda_edge e;
   leda_list<leda_edge> edges;
   leda_node edge_source_node=CG_invert.source(invert_edge);
   leda_node edge_target_node=CG_invert.target(invert_edge);
   leda_node graph_source_node=CG_invert.first_node();
   leda_node graph_sink_node=CG_invert.last_node();

   //get original source (target) node length
    int s_length, t_length;

    if(d=='H'){
      s_length=modules_info[source_label-1].width;
      t_length=modules_info[target_label-1].width;
    }
    else{
      s_length=modules_info[source_label-1].height;   
      t_length=modules_info[target_label-1].height;
   }

   //case 1: edge's source node's outdegree==1,
   //        connect edge's source node with graph's sink node. Futher, if
   //        edge's source node connect to graph's source, delete
   //        the edge which connects two nodes
   //
   if(CG_invert.outdeg(edge_source_node)==1){
       e=CG_invert.new_edge(edge_source_node, graph_sink_node);
       CG_invert.assign(e, s_length);
   }
   edges=CG_invert.in_edges(edge_source_node);

   if(edges.size()==1){
      e=edges.pop();
      if(CG_invert.source(e)==graph_source_node)
         CG_invert.del_edge(e);
   }

   //case 2: edge's target node's indegree==1,
   //        connect graph's source node with the edge's sink node. Futher, if
   //        edge's sink node connect graph's sink, delete
   //        the edge which connects two nodes

   if(CG_invert.indeg(edge_target_node)==1){
      e=CG_invert.new_edge(graph_source_node, edge_target_node);
      CG_invert.assign(e, 0);
   } 
   edges=CG_invert.out_edges(edge_target_node);

   if(edges.size()==1)
   {
       e=edges.pop();
       if(CG_invert.target(e)==graph_sink_node)
          CG_invert.del_edge(e);
   }
  
   //add the edge to another graph
   e=CG_invert.rev_edge(invert_edge);
   CG_invert.assign(e, t_length); 
   triangle_matrix.set_edge(target_label, source_label, e);
   triangle_matrix.rev_edge_type(target_label, source_label, d);
}

//operator 2::reverse edge direction in one graph
int Clo_Red_Graph::reverse_edge(GRAPH<int, int>& CG_invert,
                    GRAPH<int, int>& CG, char d)
{
   leda_edge invert_edge, e;
   leda_node source_node, target_node;
   int source_label, target_label;
   int success=1;

   success=Pick_Red_Edge(CG_invert, invert_edge);
   if(success){
     //extract the corresponding source (target) nodes from another
     //closure graph
     source_node=CG_invert.source(invert_edge);
     target_node=CG_invert.target(invert_edge);
     source_label=CG_invert[source_node];
     target_label=CG_invert[target_node];
  
     //cout<<"(R) change edge==>"<<"<"<<source_label+1<<", "
     //    <<target_label+1<<">"<<endl;

     //tune graph when we want to reverse the direction of an edge in a graph
     Tune_Graph_when_Rev(CG_invert, invert_edge, triangle_matrix, d, 
                         source_label, target_label, modules_info);
     Tune_Closure_Graphs(CG_invert, CG, target_node, source_node, d); 
     return 1;
   }
   else
     return 0;
}
//***************************************************************
//operator 3::rotate a module 
//***************************************************************
void get_node_of_label(GRAPH<int, int>& G, int label, leda_node& n)
{
    NodeIt it(G);
    int i;

   for(i=0; i<label; i++)
     ++it;
  
   n=it.get_node();
}

void assign_edges_length(GRAPH<int, int> G, leda_node& n, int length)
{
   leda_list<leda_edge> edges;
   list_item litem;
   leda_edge e;
 
   edges=G.out_edges(n);
   forall_items(litem, edges){
     e=edges[litem];
     G.assign(e, length);
   }  
}

void Clo_Red_Graph::rotate(int f){
   int size=modules_N;
   int node_label=(rand()%size)+1; 
   leda_node h_red_node, v_red_node, h_clo_node, v_clo_node;
   int width, height;
  
   //f==0, rotate positive 90 degree 
   //f==1, rotate negative 90 degree
   //cout<<"before rotate"<<endl;
   //show_modules();
   rotate_module(node_label, width, height, f);
   //cout<<"after rotate"<<endl;
   //show_modules();

   get_node_of_label(h_clo_G, node_label, h_clo_node);
   get_node_of_label(v_clo_G, node_label, v_clo_node);
   
   assign_edges_length(h_clo_G, h_clo_node, width);
   assign_edges_length(v_clo_G, v_clo_node, height);
}
//***************************************************************
//operator 4::interchang two modules 
//***************************************************************
void get_in_out_nodes(GRAPH<int, int>& G, leda_node& w1, 
     leda_list<leda_node>& in_nodes1, leda_list<leda_node>& out_nodes1, 
     leda_node& w2, leda_list<leda_node>& in_nodes2, 
     leda_list<leda_node>& out_nodes2) 
{
  leda_edge e;
  leda_node n;

  //record in nodes
  forall_in_edges(e, w1){
    n=G.source(e);
    in_nodes1.append(n);
     
    if(n!=w2)
      G.del_edge(e);
  } 
  
  //record out ndoes 
  forall_out_edges(e, w1){
    n=G.target(e);
    out_nodes1.append(n);

    if(n!=w2)
      G.del_edge(e);
  } 

  //record in nodes
  forall_in_edges(e, w2){
    n=G.source(e);
    G.del_edge(e);
    in_nodes2.append(n);
  } 
  
  //record out ndoes 
  forall_out_edges(e, w2){
    n=G.target(e);
    G.del_edge(e);
    out_nodes2.append(n);
  } 
}

void add_edges_cloG(GRAPH<int, int>& G, leda_node& w, leda_node& v,
     leda_list<leda_node>& in_nodes, leda_list<leda_node>& out_nodes,
     Modules_Info& modules_info, Hv_Matrix& triangle_matrix, char d, int modules_N)
{
   list_item litem;
   leda_edge e;
   leda_node n;
   int source_label, target_label;
    
   target_label=G[w]; 
   forall_items(litem, in_nodes)
   {
      n=in_nodes[litem];
      source_label=G[n];
      if(source_label!=target_label){
        e=G.new_edge(n, w);
        if(source_label!=0&&target_label!=modules_N+1){
          triangle_matrix.set_edge(source_label, target_label, e);
          triangle_matrix.set_edge_type(source_label, target_label, d);
        }    
        
        if(source_label!=0){
          if(d=='H')
            G.assign(e, modules_info[G[n]-1].width);
          else
            G.assign(e, modules_info[G[n]-1].height);
        }else
          G.assign(e, 0);
      }else{
        e=G.new_edge(v, w);
        source_label=G[v];
        if(source_label!=0&&target_label!=modules_N+1){ 
          triangle_matrix.set_edge(source_label, target_label, e);    
          triangle_matrix.set_edge_type(source_label, target_label, d);
        }
          
        if(d=='H')
          G.assign(e, modules_info[G[v]-1].width);
        else
          G.assign(e, modules_info[G[v]-1].height);
      }
   }  
   
   source_label=G[w];
   forall_items(litem, out_nodes)
   {
      n=out_nodes[litem];
      target_label=G[n];
      if(target_label!=source_label){
        e=G.new_edge(w, n);

        if(source_label!=0&&target_label!=modules_N+1){
           triangle_matrix.set_edge(source_label, target_label, e); 
           triangle_matrix.set_edge_type(source_label, target_label, d);
        }

        if(d=='H')
          G.assign(e, modules_info[G[w]-1].width);
        else
          G.assign(e, modules_info[G[w]-1].height);
      }
   }  
}

void Clo_Red_Graph::interchange()
{
   //list_information();
   int size=modules_N;
   int node_label1, node_label2;
   
   node_label1=(rand()%size)+1;
   do{
      node_label2=(rand()%size)+1;
   }while(node_label1==node_label2);
   //cout<<"(I) interchage node "<<node_label1+1<<"with node"
   //    <<node_label2+1<<endl;
 
   leda_node h_red_node1, h_red_node2, v_red_node1, v_red_node2;
   leda_node h_clo_node1, h_clo_node2, v_clo_node1, v_clo_node2;

   Get_Nodes_of_Labels(h_clo_G, node_label1, node_label2, 
   h_clo_node1, h_clo_node2);
   Get_Nodes_of_Labels(v_clo_G, node_label1, node_label2, 
   v_clo_node1, v_clo_node2);

   //get corresponding node of label
   leda_list<leda_node> h_clo_in_nodes1, h_clo_out_nodes1;
   leda_list<leda_node> h_clo_in_nodes2, h_clo_out_nodes2;
   leda_list<leda_node> v_clo_in_nodes1, v_clo_out_nodes1;
   leda_list<leda_node> v_clo_in_nodes2, v_clo_out_nodes2;

   //get corrsponding input and output nodes and delete the edges 
   //connecting input (output) nodes with node
   get_in_out_nodes(h_clo_G, h_clo_node1, h_clo_in_nodes1, h_clo_out_nodes1, 
                    h_clo_node2, h_clo_in_nodes2, h_clo_out_nodes2);   
   get_in_out_nodes(v_clo_G, v_clo_node1, v_clo_in_nodes1, v_clo_out_nodes1, 
                    v_clo_node2, v_clo_in_nodes2, v_clo_out_nodes2);   
      
   //add new edges and assign edge length and update matrix
   add_edges_cloG(h_clo_G, h_clo_node1, h_clo_node2, h_clo_in_nodes2, 
                  h_clo_out_nodes2, modules_info, triangle_matrix, 'H', modules_N);
   add_edges_cloG(h_clo_G, h_clo_node2, h_clo_node1, h_clo_in_nodes1, 
                  h_clo_out_nodes1, modules_info, triangle_matrix, 'H', modules_N);
   add_edges_cloG(v_clo_G, v_clo_node1, v_clo_node2, v_clo_in_nodes2, 
                  v_clo_out_nodes2, modules_info, triangle_matrix, 'V', modules_N);
   add_edges_cloG(v_clo_G, v_clo_node2, v_clo_node1, v_clo_in_nodes1, 
                  v_clo_out_nodes1, modules_info, triangle_matrix, 'V', modules_N);
}

//***************************************************************
////operator 5::move an edge from one graph to another and reverse
//***************************************************************
int Clo_Red_Graph::move_edge_reverse(GRAPH<int, int>& CG_from,
                             GRAPH<int, int>& CG_to, char d)
{
   leda_edge move_edge;
   leda_node source_node, target_node;
   int source_label, target_label;
   int success=1;

   success=Pick_Red_Edge(CG_from, move_edge);
   if(success){
     //extract the corresponding source (target) nodes from another
     //closure graph
     source_node=CG_from.source(move_edge);
     target_node=CG_from.target(move_edge);
     source_label=CG_from[source_node];
     target_label=CG_from[target_node];
     //list_information();
     //cout<<"(M) change edge==>"<<"<"<<source_label+1<<", "<<target_label+1<<">"<<endl;
     Get_Nodes_of_Labels(CG_to, target_label, source_label,
                         source_node, target_node);
    
     Tune_Graph_when_Add(CG_to, source_node, target_node, 
                         modules_info, CG_from, d, triangle_matrix);
     Tune_Closure_Graphs(CG_to, CG_from, source_node, target_node, d);  
     //list_information();  
     return 1;
   }
   else
     return 0;
}

//***************************************************************
//three permuratations with different probility
//for each operation 
//***************************************************************
void Clo_Red_Graph::permutate1()
{
   int success;
   float rotate_rate=0.7;
   int c;

   do{
      success=1;
      if(rotate_rate>rand_01()){         
         c=rand()%3; 
         switch(c){
           case 0:{   //move edges from h closure graph to v closure graph
              c=rand()%2;
              if(c){
                //cout<<"move edges from h graph to v graph"<<endl;
                success=move_edge(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph"<<endl;
                success=move_edge(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 1:{   // reverse direction of an edge in h closure graph
              c=rand()%2;
              if(c){
                //cout<<"reverse edge in h graph"<<endl;
                success=reverse_edge(h_clo_G, v_clo_G, 'H');
                break;
              }
              else{   // reverse direction of an edge in v closure graph
                //cout<<"reverse edge in v graph"<<endl;
                success=reverse_edge(v_clo_G, h_clo_G, 'V');
                break;
              }}
           case 2:{   //move edges from h closure graph to v closure graph
              c=rand()%2;
              if(c){
                //cout<<"move edges from h graph to v graph and reverse"<<endl;
                success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph and reverse"<<endl;
                success=move_edge_reverse(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
         }
      }else{
         c=rand()%2;
         switch (c) {
           case 0:
              //cout<<"interchange two nodes"<<endl;
              interchange();
              break;
           case 1:{
              c=rand()%2;
              rotate(c);
              }
         }
      }
   }while(!success);
}


void Clo_Red_Graph::permutate2()
{
   int success;
   int c;

   do{
      success=1;
      //cout<<"c="<<c<<endl;
      c=rand()%5;
        
      switch (c) {
           case 0:
              //cout<<"interchange two nodes"<<endl;
              interchange();
              break;
           case 1:{   //move edges from h closure graph to v closure graph
              c=rand()%2;
              if(c){
                //cout<<"move edges from h graph to v graph"<<endl;
                success=move_edge(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph"<<endl;
                success=move_edge(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 2:{   // reverse direction of an edge in h closure graph
              c=rand()%2;
              if(c){
                //cout<<"reverse edge in h graph"<<endl;
                success=reverse_edge(h_clo_G, v_clo_G, 'H');
                break;
              }
              else{   // reverse direction of an edge in v closure graph
                //cout<<"reverse edge in v graph"<<endl;
                success=reverse_edge(v_clo_G, h_clo_G, 'V');
                break;
              }}
           case 3:{   //move edges from h closure graph to v closure graph
              c=rand()%2;
              if(c){
                //cout<<"move edges from h graph to v graph and reverse"<<endl;
                success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph and reverse"<<endl;
                success=move_edge_reverse(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 4:{
              c=rand()%2;
              rotate(c);
              }
      }
   }while(!success);
}


void Clo_Red_Graph::permutate3()
{
   int success;
   float rotate_rate=0.2;
   int c;

   do{
      success=1;
      if(rotate_rate>rand_01()){         
         c=rand()%3; 
         switch(c){
           case 0:{   //move edges from h closure graph to v closure graph
              c=rand()%2;
              if(c){
                //cout<<"move edges from h graph to v graph"<<endl;
                success=move_edge(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph"<<endl;
                success=move_edge(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 1:{   // reverse direction of an edge in h closure graph
              c=rand()%2;
              if(c){
                //cout<<"reverse edge in h graph"<<endl;
                success=reverse_edge(h_clo_G, v_clo_G, 'H');
                break;
              }
              else{   // reverse direction of an edge in v closure graph
                //cout<<"reverse edge in v graph"<<endl;
                success=reverse_edge(v_clo_G, h_clo_G, 'V');
                break;
              }}
           case 2:{   //move edges from h closure graph to v closure graph
              c=rand()%2;
              if(c){
                //cout<<"move edges from h graph to v graph and reverse"<<endl;
                success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph and reverse"<<endl;
                success=move_edge_reverse(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
         }
      }else{
         c=rand()%2;
         switch (c) {
           case 0:
              //cout<<"interchange two nodes"<<endl;
              interchange();
              break;
           case 1:{
              c=rand()%2;
              rotate(c);
              }
         }
      }
   }while(!success);
}

//---------------------------------------------------------------
//   Simulated Annealing Temporal Solution
//---------------------------------------------------------------

float Clo_Red_Graph::get_cost(){
  return Cost;
}

void Clo_Red_Graph::get_solution(Solution &sol){
  sol.h_clo_G=h_clo_G;
  sol.v_clo_G=v_clo_G;
  sol.Area= Area;
  sol.Width=Width;
  sol.Height=Height;
  sol.Wire= Wire;
  sol.Cost = Cost;
  sol.modules_info=modules_info;
}

void copy_graph(GRAPH<int, int>& from_G, GRAPH<int, int>& G,
                Hv_Matrix& triangle_matrix, char d)
{
   G.clear();
   leda_edge e1, e2;
   leda_node n1, n2;
   int n=from_G.number_of_nodes();
   leda_array<leda_node> v(0, n-1);
   int i=0;

   forall_nodes(n1, from_G)
   {
      n2=G.new_node();
      G.assign(n2, from_G.inf(n1));
      v[i++]=n2;
   }

   forall_edges(e1, from_G)
   {
      int s_label=from_G.inf(from_G.source(e1));
      int t_label=from_G.inf(from_G.target(e1));

      e2=G.new_edge(v[s_label], v[t_label]);
      G.assign(e2, from_G.inf(e1));

      if(s_label!=0&&t_label!=n-1)
      { 
        triangle_matrix.set_edge(s_label, t_label, e2);
        triangle_matrix.set_edge_type(s_label, t_label, d);
      }
   }
}

void Clo_Red_Graph::recover(Solution &sol){
  copy_graph(sol.h_clo_G, h_clo_G, triangle_matrix, 'H');
  copy_graph(sol.v_clo_G, v_clo_G, triangle_matrix, 'V');
  Area=sol.Area;
  Wire=sol.Wire;
  Width=sol.Width;
  Height=sol.Height;
  Cost=sol.Cost;
  modules_info=sol.modules_info;
}
//---------------------------------------------------------------
//   Debug Tools
//---------------------------------------------------------------
void Clo_Red_Graph::Test()
{
   leda_edge e;
   int source_label, target_label;
    
   forall_edges(e, h_clo_G)
   {
       source_label=h_clo_G[h_clo_G.source(e)];
       target_label=h_clo_G[h_clo_G.target(e)];
       
       if(source_label!=0&&target_label!=modules_N+1)
       {   
            if(!triangle_matrix.equal_type(source_label, target_label, 'H'))
            {
              cout<<"type error of h_clo_G in clo_red.cc!!!"<<endl;
              cout<<"<"<<source_label+1<<", "<<target_label+1<<">"<<endl;
              list_information();
              exit(0);
            }
       }
      
       int length=h_clo_G[e];
       if(source_label==0){
          if(length!=0){
             cout<<"Length Error of h_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!"<<endl;
             list_information();
             exit(0);
          }
       }else{
          if(length!=modules_info[source_label-1].width){
             cout<<"Length Error of h_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!"<<endl;
             list_information();
             exit(0);
          }
       }  
   }


   forall_edges(e, v_clo_G)
   {
       source_label=v_clo_G[v_clo_G.source(e)];
       target_label=v_clo_G[v_clo_G.target(e)];
       
       if(source_label!=0&&target_label!=modules_N+1)
       {   
            if(!triangle_matrix.equal_type(source_label, target_label, 'V'))
            {
               cout<<"type error of v_clo_G in clo_red.cc!!!"<<endl;
               cout<<"<"<<source_label+1<<", "<<target_label+1<<">"<<endl;
               list_information();
               exit(0);
            }
       }

       int length=v_clo_G[e];
       if(source_label==0){
          if(length!=0){
             cout<<"Length Error of v_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!"<<endl;
             list_information();
             exit(0);
          }
       }else{
          if(length!=modules_info[source_label-1].height){
             cout<<"Length Error of v_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!"<<endl;
             list_information();
             exit(0);
          }
       }  

   }

}

void Clo_Red_Graph::list_information()
{
      cout<<"*******************************************"<<endl;
      cout<<"transitive closure horizontal graph"<<endl;
      cout<< h_clo_G;

      cout<<"*******************************************"<<endl;
      cout<<"transitive closure vertical graph"<<endl;
      cout<<v_clo_G;

      cout<<"*******************************************"<<endl;
      cout<<triangle_matrix;

      cout<<"*******************************************"<<endl;
      cout<<"Best Area="<<get_cost()<<endl;
}

float rand_01(){
  return float(rand()%10000)/10000;
}
