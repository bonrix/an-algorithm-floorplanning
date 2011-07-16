#include <LEDA/array.h>
#include <LEDA/graph_iterator.h>
#include <LEDA/list.h>
#include <vector>
#include <math.h>
#include <time.h>
#include "top_sort.h"
#include "clo_red.h"
#include "show_graph.h"
//-------------------------------------------------------
void print_message(GRAPH<int, float>& G)
{
   leda_edge e;
   forall_edges(e, G)
   {
      int s_l, t_l;
      cout<<"<"<<G[G.source(e)]<<", ";
      cout<<G[G.target(e)]<<", ";
      cout<<G[e]<<">"<<endl;
   }
}

void copy_graph(GRAPH<int, float>& from_G, GRAPH<int, float>& G,
                Hv_Matrix& triangle_matrix, char d, vector<leda_node>& nodes)
{
   G.clear();
   leda_edge e1, e2;
   leda_node n1, n2;
   int n=from_G.number_of_nodes();
//   leda_array<leda_node> v(0, n-1);
   int i=0;

   forall_nodes(n1, from_G)
   {
      n2=G.new_node();
      G.assign(n2, from_G.inf(n1));
      nodes[i++]=n2;   
//      nodes[from_G.inf(n1)]=n2;   

   }

   forall_edges(e1, from_G)
   {
      int s_label=from_G.inf(from_G.source(e1));
      int t_label=from_G.inf(from_G.target(e1));

      e2=G.new_edge(nodes[s_label], nodes[t_label]);
      G.assign(e2, from_G.inf(e1));

      if(s_label!=0&&t_label!=n-1)
      { 
        triangle_matrix.set_edge(s_label, t_label, e2);
        triangle_matrix.set_edge_type(s_label, t_label, d);
      }
   }
}

//---------------------------------------------------------------
// Initialization
//---------------------------------------------------------------
void Clo_Red_Graph::init()
{
   int i, j;
   leda_node n;
   leda_edge e;
   int module_size=modules_N;
   triangle_matrix.init(module_size);
   leda_list<int> node_array;
   vector<int> sequence;
   sequence.resize(module_size+2);
   h_nodes.resize(module_size+2);
   v_nodes.resize(module_size+2);

   for(i=1; i<= module_size; i++)
       node_array.append(i); 
   
   i=0;
   int mod;
   int sub1, sub2;
   sequence[i++]=0; 
   do{
      node_array.permute();
      mod=node_array.pop();
         
      if(modules[mod-1].type==0){
        sequence[i++]=mod;
      }else if(modules[mod-1].type==2){
        sequence[i++]=mod;
        sub1=modules[mod-1].submodules[0].id+1;
        sequence[i++]=sub1;
        node_array.remove(sub1);
      }else{
         sub1=modules[mod-1].submodules[0].id+1;
         sub2=modules[mod-1].submodules[1].id+1; 

         if(mod<sub1){
           sequence[i++]=mod;
           sequence[i++]=sub1;
           sequence[i++]=sub2;
         }else if(mod<sub2){
           sequence[i++]=sub1;
           sequence[i++]=mod; 
           sequence[i++]=sub2;
         }else{
           sequence[i++]=sub1;
           sequence[i++]=sub2; 
           sequence[i++]=mod;
         }
         node_array.remove(sub1);
         node_array.remove(sub2);
      }
   }while(node_array.size()>0); 
   sequence[i]=module_size+1;
/*   
   for(i=0; i<=module_size+1; i++)
   {  
       //sequence[i]=i;
       cout<<sequence[i]<< " ";
   }
   cout<<endl;      
*/  
   //construct transitive closure for horizontal (vertical) graph
   //node 0 (module_size+1) denote source (sink) node

   for(i=0; i <= module_size+1; i++)
   {
      n=h_clo_G.new_node();
      h_nodes[i]=n;
      h_clo_G.assign(n, i);

      n=v_clo_G.new_node();
      v_nodes[i]=n;
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
            e=h_clo_G.new_edge(h_nodes[sequence[i]], 
                               h_nodes[sequence[j]]);
            h_clo_G.assign(e, modules[sequence[i]-1].width);

            triangle_matrix.set_edge(sequence[i], sequence[j], e);
            if(sequence[i]<sequence[j])
               triangle_matrix.set_edge_type(sequence[i], sequence[j], H);
            else 
               triangle_matrix.set_edge_type(sequence[j], sequence[i], h);
         }
      }
      else
      {
        e=h_clo_G.new_edge(h_nodes[sequence[i]], 
                           h_nodes[sequence[i+1]]);

        if(i==0)
           h_clo_G.assign(e, 0);
        else
           h_clo_G.assign(e, modules[sequence[i]-1].width);
       }

      //vertical graph
      if(i!=module_size)
      {
        e=v_clo_G.new_edge(v_nodes[0], v_nodes[sequence[i+1]]);
        v_clo_G.assign(e, 0);

        e=v_clo_G.new_edge(v_nodes[sequence[i+1]], 
                           v_nodes[module_size+1]);
        v_clo_G.assign(e, modules[sequence[i+1]-1].height);
      }
   }
   //list_information();
}

void Clo_Red_Graph::init_square()
{
   int i, j;
   leda_node n;
   leda_edge e;
   int module_size=modules_N;
   triangle_matrix.init(module_size);
   leda_list<int> node_array;
   vector<int> sequence;
   sequence.resize(module_size+2);
   h_nodes.resize(module_size+2);
   v_nodes.resize(module_size+2);

   for(i=1; i<= module_size; i++)
       node_array.append(i); 
   
   i=0;
   int mod;
   int sub1, sub2;
   sequence[i++]=0; 
   do{
      node_array.permute();
      mod=node_array.pop();
         
      if(modules[mod-1].type==0){
        sequence[i++]=mod;
      }else if(modules[mod-1].type==2){
        sequence[i++]=mod;
        sub1=modules[mod-1].submodules[0].id+1;
        sequence[i++]=sub1;
        node_array.remove(sub1);
      }else{
         sub1=modules[mod-1].submodules[0].id+1;
         sub2=modules[mod-1].submodules[1].id+1; 

         if(mod<sub1){
           sequence[i++]=mod;
           sequence[i++]=sub1;
           sequence[i++]=sub2;
         }else if(mod<sub2){
           sequence[i++]=sub1;
           sequence[i++]=mod; 
           sequence[i++]=sub2;
         }else{
           sequence[i++]=sub1;
           sequence[i++]=sub2; 
           sequence[i++]=mod;
         }
         node_array.remove(sub1);
         node_array.remove(sub2);
      }
   }while(node_array.size()>0); 
   sequence[i]=module_size+1;

   //construct transitive closure for horizontal (vertical) graph
   //node 0 (module_size+1) denote source (sink) node

   for(i=0; i <= module_size+1; i++)
   {
      n=h_clo_G.new_node();
      h_nodes[i]=n;
      h_clo_G.assign(n, i);

      n=v_clo_G.new_node();
      v_nodes[i]=n;
      v_clo_G.assign(n, i);
   }

   //construct edges in horizontal (vertical) transitive graphs
   int k, l=1, over=0, change=0;
   int len=(int)ceil(sqrt(modules_N));
   //int temp=len+1;

   for(i=1; i <modules_N; i++){
      l=((i-1-over)%len)+1;
      //over=0;
      if ((l==len || l==0) && change==1)
      {
      	 l=0;
      	 change=0;
      }
      for(j=i+1; j<= modules_N; j++){
         if(l<len){
            e=h_clo_G.new_edge(h_nodes[sequence[i]], h_nodes[sequence[j]]);
            h_clo_G.assign(e, modules[sequence[i]-1].width);

            triangle_matrix.set_edge(sequence[i], sequence[j], e);
            triangle_matrix.set_edge_type(sequence[i], sequence[j], 'H');

            if ( (modules[sequence[j]-1].type==2 && modules[sequence[j]-1].submodules[0].id+1==sequence[j+1]) 
               || (modules[sequence[j]-1].type==3 && modules[sequence[j]-1].submodules[1].id+1==sequence[j+1]) )
             
            {
            	j++;
            	l++;
            	e=h_clo_G.new_edge(h_nodes[sequence[i]], h_nodes[sequence[j]]);
            	h_clo_G.assign(e, modules[sequence[i]-1].width);
            	
            	triangle_matrix.set_edge(sequence[i], sequence[j], e);
            	triangle_matrix.set_edge_type(sequence[i], sequence[j], 'H');

            	if (l>=len)
            	  over+=l-len+1;
            }
            else if (modules[sequence[j]-1].type==3 && modules[sequence[j]-1].submodules[0].id+1==sequence[j+1])
            {
            	j++;
            	l++;
            	e=h_clo_G.new_edge(h_nodes[sequence[i]], h_nodes[sequence[j]]);
            	h_clo_G.assign(e, modules[sequence[i]-1].width);

            	triangle_matrix.set_edge(sequence[i], sequence[j], e);
            	triangle_matrix.set_edge_type(sequence[i], sequence[j], 'H');

            	j++;
            	l++;
            	e=h_clo_G.new_edge(h_nodes[sequence[i]], h_nodes[sequence[j]]);
            	h_clo_G.assign(e, modules[sequence[i]-1].width);

            	triangle_matrix.set_edge(sequence[i], sequence[j], e);
            	triangle_matrix.set_edge_type(sequence[i], sequence[j], 'H');

            	if (l>=len)
            	{
            	  over+=l-len+1;
            	  if (l>len)
            	  {
            	    change=1;
            	  }
            	}
            }
            //else if (l=len-1) change==1;
         }else{
            e=v_clo_G.new_edge(v_nodes[sequence[i]], v_nodes[sequence[j]]);
            v_clo_G.assign(e, modules[sequence[i]-1].height);

            triangle_matrix.set_edge(sequence[i], sequence[j], e);
            triangle_matrix.set_edge_type(sequence[i], sequence[j], 'V');
            //change=0;
         }
         l++;
      }//end of for j
      
   }
   
   forall_nodes(n, h_clo_G){
      if(h_clo_G.indeg(n)==0 && n!=h_clo_G.first_node() && n!=h_clo_G.last_node()){
      //if(h_clo_G.indeg(n)==0){
         e=h_clo_G.new_edge(h_nodes[0], h_nodes[h_clo_G[n]]);
         h_clo_G.assign(e, 0);
      }

      if(h_clo_G.outdeg(n)==0 && n!=h_clo_G.last_node()&& n!=h_clo_G.first_node()){
      //if(h_clo_G.outdeg(n)==0){
         e=h_clo_G.new_edge(h_nodes[h_clo_G[n]], h_nodes[modules_N+1]);
         h_clo_G.assign(e, modules[h_clo_G[n]-1].width);
      }
   }

   forall_nodes(n, v_clo_G){
      if(v_clo_G.indeg(n)==0 && n!=v_clo_G.first_node() && n!=v_clo_G.last_node()){
      //if(v_clo_G.indeg(n)==0){
         e=v_clo_G.new_edge(v_nodes[0], v_nodes[v_clo_G[n]]);
         v_clo_G.assign(e, 0);
      }

      if(v_clo_G.outdeg(n)==0 && n!=v_clo_G.last_node() && n!=v_clo_G.first_node()){
      //if(v_clo_G.outdeg(n)==0){
         e=v_clo_G.new_edge(v_nodes[v_clo_G[n]], v_nodes[modules_N+1]);
         v_clo_G.assign(e, modules[v_clo_G[n]-1].height);
      }
   }  
   //Debug();
   //list_information();
   //packing();
   //show_graph(getWidth(), getHeight(), modules_info);
   //exit(0);
}
//---------------------------------------------------------------
// Placement Modules
//---------------------------------------------------------------
//compute longest path in acyclic graph
float Clo_Red_Graph::Acyclic_Longest_Path(GRAPH<int, float>& G,  char d, vector<leda_node>& G_nodes)
{
   leda_edge e;

   //acyclic graph: establish topological order
   leda_node_array<int> top_ord(G);
   
   Packing_Topologic_Sort(G, modules, top_ord, G_nodes, d, modules_info, triangle_matrix); //top_ord is now a topological ordering of G
     
   int n=G.number_of_nodes();
   leda_array<leda_node> v(1, n);
   leda_node w;
   leda_node_array<float> dist(G);
   leda_node_array<leda_edge> pred(G);
   vector<int> Proccessed;
   Proccessed.resize(n);
   forall_nodes(w, G)
   {
      v[top_ord[w]]=w;  //top_ord[v[i]]==i for all i
      dist[w]=-1;
      pred[w]=nil;
      Proccessed[G[w]]=0;
   }

   //relax each vertex in topological order
   leda_node s=G.first_node();
   dist[s]=0;
   for(int i=top_ord[s]; i <= n; i++)
   {
       leda_node u=v[i];
       if(pred[u]==nil && u!= s) continue;
       
       leda_edge e;
       float du;
       float length;
       int label=G[u],sub_label,sub_label1,sub_label2;
       int index=label-1;
       leda_node sub_node,sub_node1,sub_node2;
       if (u!=G.last_node())
       {
          if (Proccessed[label]==0 && index>=0 && modules[index].type!=0)
          {
	     if (triangle_matrix.get_edge_type_c(label, modules[index].submodules[0].id+1)!=d)
             {
                if (modules[index].type==2)
                {
        	   sub_label=modules[index].submodules[0].id+1;
        	   sub_node=G_nodes[sub_label];
       	           if (modules_info[index].relation==0)
        	   {
       	              dist[u]=max(dist[u], dist[sub_node]-modules[index].submodules[0].relation1);
       	              dist[sub_node]=dist[u]+modules[index].submodules[0].relation1;
       	           }
       	           else
       	           {
       	              dist[u]=max(dist[u], dist[sub_node]-modules[index].submodules[0].relation2);
       	              dist[sub_node]=dist[u]+modules[index].submodules[0].relation2;
       	           }
       	           Proccessed[label]=1;
       	           Proccessed[sub_label]=1;
                }
                else if(modules[index].type==3)
                {
       	           sub_label1=modules[index].submodules[0].id+1;
       	           sub_node1=G_nodes[sub_label1];
       	           sub_label2=modules[index].submodules[1].id+1;
       	           sub_node2=G_nodes[sub_label2];
       	           float temp;
       	           if (modules_info[index].relation==0)
       	           {
       	              temp=max(dist[u], dist[sub_node1]-modules[index].submodules[0].relation1);
       	              dist[u]=max(temp, dist[sub_node2]-modules[index].submodules[1].relation1);
       	              dist[sub_node1]=dist[u]+modules[index].submodules[0].relation1;
       	              dist[sub_node2]=dist[u]+modules[index].submodules[1].relation1;
       	           }
       	           else
       	           {
       	              temp=max(dist[u], dist[sub_node1]-modules[index].submodules[0].relation2);
       	              dist[u]=max(temp, dist[sub_node2]-modules[index].submodules[1].relation2);
       	              dist[sub_node1]=dist[u]+modules[index].submodules[0].relation2;
       	              dist[sub_node2]=dist[u]+modules[index].submodules[1].relation2;
       	           }
       	           Proccessed[label]=1;
       	           Proccessed[sub_label1]=1;
       	           Proccessed[sub_label2]=1;
                }
	     }//if relations need to be concerned
          } 
       }//u!=G.last_node()  
       du=dist[u];
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
       
      if(label!=0&& w!=G.last_node()){ 
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
   Width  = Acyclic_Longest_Path(h_clo_G, 'H', h_nodes);
   //cout<<"width -> "<<Width<<endl;
   Height = Acyclic_Longest_Path(v_clo_G, 'V', v_nodes);   
   //cout<<"height -> "<<Height<<endl;
   Area   = Height*Width; 
   
   //Compute_Wire_Length();

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
   //Debug();
   //Test();
}

void Clo_Red_Graph::Compute_Wire_Length(){
  float length=0;

  // int num_of_pins=0;
  // compute absolute position
  int s=modules_info.size();

  for(int i=0; i < s; i++){
    float mx= modules_info[i].x, my= modules_info[i].y;
    float width=Pmodules[modules[i].id].width;
    float height=Pmodules[modules[i].id].height;

  // num_of_pins+=modules[i].pins.size();
    for(int j=0; j < Pmodules[modules[i].id].pins.size(); j++){
      Pin &p = Pmodules[modules[i].id].pins[j];
      float p_x, p_y;

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

  float x_ratio=(float)Width/Pmodules[s].width;
  float y_ratio=(float)Height/Pmodules[s].height;
  for(int j=0; j < Pmodules[s].pins.size(); j++){
      Pin &p = Pmodules[s].pins[j];
      p.ax=int(p.x*x_ratio);
      p.ay=int(p.y*y_ratio);
  }

  for(int i=0; i < network.size(); i++){
    assert(network[i].size() > 0);
    float max_x= INT_MIN, max_y= INT_MIN;
    float min_x= INT_MAX, min_y= INT_MAX;

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

void Clo_Red_Graph::Compute_Cost(float alpha, float beta, Solution& Best)
{
  
   if(alpha!=0.0&&beta!=0.0)
     Cost=alpha*Area/Best.Area+beta*Wire/Best.Wire;
   else if(alpha==0.0&&beta!=0.0)
     Cost=Wire;
   else if(alpha!=0.0&&beta==0.0)
     Cost=Area;

}
//---------------------------------------------------------------------------
//   Simulated Annealing Permutation Operations
//---------------------------------------------------------------------------
int Clo_Red_Graph::Pick_Red_Edge(GRAPH<int, float>& G, leda_edge& pick_e, leda_node& pick_node)
{
   leda_list<leda_node> nodes;
   int iteration;
   leda_node u, w;
   leda_edge e;
   leda_node sink_node=G.last_node();
   int node_index=G[pick_node]-1;
   forall_out_edges(e, pick_node)
   {
      u=G.target(e);
      if (u!=sink_node)
      {   
         nodes.append(u);
      }

   }

   iteration=nodes.size();
   while (iteration>0 && nodes.size()>1)
   {
      u=nodes.pop();
      forall_out_edges(e ,u)
      {
      	 w=G.target(e);
      	 nodes.remove(w);
      }
      iteration--;
      nodes.append(u);
   }
   
   if (nodes.size()==0)
      return 0;
   else
   {
      int offset;
      offset=rand()%nodes.size();
      u=nodes[nodes[offset]];
      if (modules[node_index].type==0)
      {
      	 pick_e=triangle_matrix.get_edge(G[pick_node],G[u]);
         return 1;
      }
      else if (modules[node_index].type==2)
      {
      	 int sub_index=modules[node_index].submodules[0].id;
      	 while (nodes.size()>0)
         {
            if (sub_index!=G[u]-1)
            {
                pick_e=triangle_matrix.get_edge(G[pick_node],G[u]);  
                return 1;
            }
            else
            {
                nodes.remove(u);
                if(nodes.size()<1)
                  return 0;
                else
                {
                  offset=rand()%nodes.size();
                  u=nodes[nodes[offset]];
                }
            }
         }
         return 0;
      }
      else
      {
         int sub_index1=modules[node_index].submodules[0].id;
         int sub_index2=modules[node_index].submodules[1].id;
      	 while (nodes.size()>0)
         {
            if (sub_index1!=G[u]-1 && sub_index2!=G[u]-1)
            {
                pick_e=triangle_matrix.get_edge(G[pick_node],G[u]);  
                return 1;
            }
            else
            {
                nodes.remove(u);
		if (nodes.size()<1)
		  return 0;
		else
		{
                   offset=rand()%nodes.size();
                   u=nodes[nodes[offset]];
                }
            }
         }
         return 0;
      } //if type==0  
   }//if nodes.size!=0
   if (G.source(pick_e)==G.first_node() || G.target(pick_e)==G.last_node())
   {
   	cout<<"Pick_Red_Edge error !!! "<<endl;
   	exit(0);
   }

} 
//***********************************************************************************
//Check if involve a rectilinear edge and compute edges need moved when move a edge
//***********************************************************************************
int Clo_Red_Graph::Compute_Move_Edges(GRAPH<int, float>& CG_to,
                                      GRAPH<int, float>& CG_from,
                                      leda_node edge_source_node,
                                      leda_node edge_target_node, 
                                      leda_list<leda_node>& Move_Edge_List, char d)
{   int edge_source_label=CG_to[edge_source_node];
    int edge_target_label=CG_to[edge_target_node];
    int edge_length;
    leda_edge e;
    int num_of_nodes=CG_to.number_of_nodes();
    
    //incremental update edges
    leda_edge se, te;
    leda_node sn, tn;
    int s_label, t_label, temps, tempt;
    //CASE1 
    forall_in_edges(se, edge_source_node){
      sn=CG_to.source(se);
      s_label=CG_to[sn];    
      if(s_label!=0)
      {  
        temps=s_label-1;
        tempt=edge_target_label-1;
        if (modules[temps].type==2 && modules[tempt].type==2 && modules[temps].submodules[0].id==tempt)
           return 0;
        else if (modules[temps].type==3 && modules[tempt].type==3)
        {
           if (temps==tempt-1 && (temps==modules[tempt].submodules[0].id || temps==modules[tempt].submodules[1].id))
             return 0;
           else if (tempt==temps-1 && (tempt==modules[temps].submodules[0].id || tempt==modules[temps].submodules[1].id)) 
             return 0;
        }
        if(triangle_matrix.get_edge_type_c(s_label, edge_target_label)!=d)   
        {  
          Move_Edge_List.append(sn);
          Move_Edge_List.append(edge_target_node);   
        }
      }
    }
    
    //CASE1
    forall_out_edges(te, edge_target_node){
      tn=CG_to.target(te);
      t_label=CG_to[tn];    
      if(t_label!=(num_of_nodes-1))
      {        
        temps=edge_source_label-1;
        tempt=t_label-1;
        if (modules[temps].type==2 && modules[tempt].type==2 && modules[temps].submodules[0].id==tempt)
           return 0;
        else if (modules[temps].type==3 && modules[tempt].type==3)
        {
           if (temps==tempt-1 && (temps==modules[tempt].submodules[0].id || temps==modules[tempt].submodules[1].id))
              return 0;
           else if (tempt==temps-1 && (tempt==modules[temps].submodules[0].id || tempt==modules[temps].submodules[1].id)) 
              return 0;      
        }
        if(triangle_matrix.get_edge_type_c(edge_source_label, t_label)!=d)
        {  
          Move_Edge_List.append(edge_source_node);
          Move_Edge_List.append(tn);
        }
      }
    }
    
    //CASE1
    forall_in_edges(se, edge_source_node){
      sn=CG_to.source(se);
      s_label=CG_to[sn];
      forall_out_edges(te, edge_target_node){
        tn=CG_to.target(te);
        t_label=CG_to[tn];    
        if(s_label!=0&&t_label!=(num_of_nodes-1))
        {
          temps=s_label-1;
          tempt=t_label-1;
          if (modules[temps].type==2 && modules[tempt].type==2 && modules[temps].submodules[0].id==tempt)
             return 0;
          else if (modules[temps].type==3 && modules[tempt].type==3)
          {
             if (temps==tempt-1 && (temps==modules[tempt].submodules[0].id || temps==modules[tempt].submodules[1].id))
                return 0;
             else if (tempt==temps-1 && (tempt==modules[temps].submodules[0].id || tempt==modules[temps].submodules[1].id)) 
                return 0;
          }
          if(triangle_matrix.get_edge_type_c(s_label, t_label)!=d)
          {        
             Move_Edge_List.append(sn);
             Move_Edge_List.append(tn);
          }//if !=d
        }
      }
    }     
    return 1;	
}


void Clo_Red_Graph::Revese_relation(int index)
{
   if (modules_info[index].relation==0)
      modules_info[index].relation=1;
   else
      modules_info[index].relation=0;
}

//***************************************************************
////operator 1::move an edge from one graph to another
//***************************************************************
//Tuning the graph "trans_closure_G" when we add an edge to a new graph.
void Tune_Graph_when_Add(GRAPH<int, float>& CG_to,
     leda_node& edge_source_node, leda_node& edge_target_node, 
     Modules_Info& modules_info, GRAPH<int, float>& CG_from, char d,
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
    float edge_length;
    if(d=='H')
       edge_length=modules_info[edge_source_label-1].width;
    else
       edge_length=modules_info[edge_source_label-1].height;

    e=CG_to.new_edge(edge_source_node, edge_target_node);
    CG_to.assign(e, edge_length);
    triangle_matrix.Update_Matrix(edge_source_label, edge_target_label, d, 
                                  CG_from, e);
}

//check fan-in nodes and fan-out nodes
void Clo_Red_Graph::Tune_Closure_Graphs(GRAPH<int, float>& CG_to,
                                        GRAPH<int, float>& CG_from,
                                        leda_node edge_source_node,
                                        leda_node edge_target_node, char d)
{
    int edge_source_label=CG_to[edge_source_node];
    int edge_target_label=CG_to[edge_target_node];
    float edge_length;
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

// 'd' denote CG_to 
int Clo_Red_Graph::move_edge(GRAPH<int, float>& CG_from,
                             GRAPH<int, float>& CG_to, char d)
{
   leda_edge move_edge;
   leda_node source_node, target_node, pick_node;
   int source_label, target_label;
   int success=0,count=0;
   
   while (success==0 && count<2)
   {      
      pick_node=CG_from.choose_node();
      while (pick_node==CG_from.first_node() || pick_node==CG_from.last_node())
          pick_node=CG_from.choose_node();
      success=Pick_Red_Edge(CG_from, move_edge, pick_node);
      count++;
   }
   
   if(success){
     
     //extract the corresponding source (target) nodes from another
     //closure graph
     source_node=CG_from.source(move_edge);
     target_node=CG_from.target(move_edge);
     source_label=CG_from[source_node];
     target_label=CG_from[target_node];
     
     //cout<<"(M) change edge==>"<<"<"<<source_label+1<<", "<<target_label+1<<">"<<endl;
     if(d=='H'){
        source_node=h_nodes[source_label];
        target_node=h_nodes[target_label];
      }else{
        source_node=v_nodes[source_label];
        target_node=v_nodes[target_label];
      }
             
     //cout<<"start move edge "<<source_label+1<<" "<<target_label+1<<" destination "<<d<<endl;     
     leda_list<leda_node> Move_Edge_List;
     leda_edge e;
     int OK=Compute_Move_Edges(CG_to, CG_from, source_node, target_node, Move_Edge_List, d);
     if(OK==1)
     {
     	Tune_Graph_when_Add(CG_to, source_node, target_node, modules_info, CG_from, d, triangle_matrix);
     	int s_label, t_label;
     	float edge_length;
     	list_item litem, Sitem, Titem;

     	int i=0;	 
      	forall_items(litem, Move_Edge_List)
      	{  
          if(i%2==0){
            Sitem=litem; 
          }
          else {
            Titem=litem; 
            s_label=CG_to[Move_Edge_List[Sitem]];
            t_label=CG_to[Move_Edge_List[Titem]];
            if(d=='H')
              edge_length=modules_info[s_label-1].width;
            else
              edge_length=modules_info[s_label-1].height;
      
            e=CG_to.new_edge(Move_Edge_List[Sitem], Move_Edge_List[Titem]);
            CG_to.assign(e, edge_length);
            triangle_matrix.Update_Matrix(s_label, t_label, d, CG_from, e);
          }
          i++; 		
        }
        return 1;
     }
     else
        return 0;
   }
   else
     return 0;
}

//***************************************************************
////operator 2::reverse edge direction in one graph
//***************************************************************
void Tune_Graph_when_Rev(GRAPH<int, float>& CG_invert,
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
    float s_length, t_length;

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
int Clo_Red_Graph::reverse_edge(GRAPH<int, float>& CG_invert,
                    GRAPH<int, float>& CG, char d)
{
   leda_edge invert_edge, e;
   leda_node source_node, target_node, pick_node;
   int source_label, target_label;
   int success=0,count=0;

   while (success==0 && count<2)
   {
      pick_node=CG_invert.choose_node();
      while (pick_node==CG_invert.first_node() || pick_node==CG_invert.last_node())
         pick_node=CG_invert.choose_node();
      success=Pick_Red_Edge(CG_invert, invert_edge, pick_node);
      count++;
   }

   if(success){
     
     //extract the corresponding source (target) nodes from another
     //closure graph
     source_node=CG_invert.source(invert_edge);
     target_node=CG_invert.target(invert_edge);
     source_label=CG_invert[source_node];
     target_label=CG_invert[target_node];

     //cout<<"start reverse edge "<<source_label+1<<" "<<target_label+1<<"  destination "<<d<<endl;     
     float length=CG_invert[invert_edge];
     leda_list<leda_node> Move_Edge_List;
     CG_invert.del_edge(invert_edge);//avoid cycle during checking.
     int OK=Compute_Move_Edges(CG_invert, CG, target_node, source_node, Move_Edge_List, d);
     invert_edge=CG_invert.new_edge(source_node, target_node);
     triangle_matrix.set_edge(source_label, target_label, invert_edge);
     CG_invert[invert_edge]=length;
     if(OK==1)
     {
	Tune_Graph_when_Rev(CG_invert, invert_edge, triangle_matrix, d, 
				source_label, target_label, modules_info);
     	int s_label, t_label;
     	float edge_length;
     	list_item litem, Sitem, Titem;

     	int i=0;	 
      	forall_items(litem, Move_Edge_List)
      	{  
          if(i%2==0){
            Sitem=litem; 
          }
          else {
            Titem=litem; 
            s_label=CG_invert[Move_Edge_List[Sitem]];
            t_label=CG_invert[Move_Edge_List[Titem]];
            if(d=='H')
              edge_length=modules_info[s_label-1].width;
            else
              edge_length=modules_info[s_label-1].height;
      
            e=CG_invert.new_edge(Move_Edge_List[Sitem], Move_Edge_List[Titem]);
            CG_invert.assign(e, edge_length);
            triangle_matrix.Update_Matrix(s_label, t_label, d, CG, e);
          }
          i++; 		
        }
        return 1;
     }
     else
	return 0;         
   }
   else
     return 0;
}
//***************************************************************
//operator 3::rotate a module 
//***************************************************************
void get_node_of_label(GRAPH<int, float>& G, int label, leda_node& n)
{
    NodeIt it(G);
    int i;

   for(i=0; i<label; i++)
     ++it;
  
   n=it.get_node();
}

void assign_edges_length(GRAPH<int, float> G, leda_node& n, float length)
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

int Clo_Red_Graph::rotate(int f){
   int size=modules_N;
   int node_label=(rand()%size)+1; 
   leda_node h_red_node, v_red_node, h_clo_node, v_clo_node;
   float width, height;
   leda_edge move_edge;
   //f==0, rotate positive 90 degree 
   //f==1, rotate negative 90 degree
 
   int index=node_label-1;
   char s;
   leda_node source_node, target_node;
   int success=0;
   
   if (modules[index].type==0)
   {
      //cout<<"rotate hard module"<<"  -> "<<index+1<<endl;
      rotate_module(node_label, width, height, f);
      h_clo_node=h_nodes[node_label];
      v_clo_node=v_nodes[node_label];
   
      assign_edges_length(h_clo_G, h_clo_node, width);
      assign_edges_length(v_clo_G, v_clo_node, height);
       return 1;
   }
   else if(modules[index].type==2)
   {
      //cout<<"rotate L-shaped module"<<"  -> "<<index+1<<endl;
      int source_label, target_label;
      move_edge=triangle_matrix.get_edge(index+1, modules[index].submodules[0].id+1); 
      s=triangle_matrix.get_edge_type_c(index+1, modules[index].submodules[0].id+1);
      leda_list<leda_node> Move_Edge_List; 
      if (s=='H')
      {
         source_node=h_clo_G.source(move_edge);
         target_node=h_clo_G.target(move_edge);
         source_label=h_clo_G[source_node];
         target_label=h_clo_G[target_node];
         success=Rotate_Rectilinear(h_clo_G, v_clo_G, source_label, target_label, 'V', move_edge, Move_Edge_List);
         if (success)
         {
	    Move_Edges(h_clo_G, v_clo_G, source_label, target_label, 'V', move_edge, Move_Edge_List);
	    node_label=source_label;
            rotate_module(node_label, width, height, 1);
            h_clo_node=h_nodes[node_label];
            v_clo_node=v_nodes[node_label];
            assign_edges_length(h_clo_G, h_clo_node, width);
            assign_edges_length(v_clo_G, v_clo_node, height);
         
            node_label=target_label;
            rotate_module(node_label, width, height, 1);
            h_clo_node=h_nodes[node_label];
            v_clo_node=v_nodes[node_label];
            assign_edges_length(h_clo_G, h_clo_node, width);
            assign_edges_length(v_clo_G, v_clo_node, height);
            return 1;
         }
         else
            return 0;
      }
      else{
         source_node=v_clo_G.source(move_edge);
         target_node=v_clo_G.target(move_edge);
         source_label=v_clo_G[source_node];
         target_label=v_clo_G[target_node];
         
         success=Rotate_Rectilinear(v_clo_G, h_clo_G, source_label, target_label, 'H', move_edge, Move_Edge_List);
	 if (success)
	 {
	    Move_Edges(v_clo_G, h_clo_G, source_label, target_label, 'H', move_edge, Move_Edge_List);
	    node_label=source_label;
            rotate_module(node_label, width, height, 0);
            h_clo_node=h_nodes[node_label];
            v_clo_node=v_nodes[node_label];
            assign_edges_length(h_clo_G, h_clo_node, width);
            assign_edges_length(v_clo_G, v_clo_node, height);
         
            node_label=target_label;
            rotate_module(node_label, width, height, 0);
            h_clo_node=h_nodes[node_label];
            v_clo_node=v_nodes[node_label];
            assign_edges_length(h_clo_G, h_clo_node, width);
            assign_edges_length(v_clo_G, v_clo_node, height);
            return 1;
         }
         else 
            return 0;
      }
               
   }
   else if(modules[index].type==3)
   {   
      //cout<<"rotate T-shaped module "<<" -> "<<index+1<<endl;
      int label_list[3];

      if (index<modules[index].submodules[0].id)
      {
      	 label_list[0]=index+1;
      	 label_list[1]=index+2;
      	 label_list[2]=index+3;
      }
      else if (index>modules[index].submodules[1].id) 	
      {
      	 label_list[0]=index-1;
      	 label_list[1]=index;
      	 label_list[2]=index+1;
      }
      else
      {
      	 label_list[0]=index;
      	 label_list[1]=index+1;
      	 label_list[2]=index+2;
      }
      //cout <<label_list[0]<<label_list[1]<<label_list[2]<<endl; 
      s=triangle_matrix.get_edge_type_c(label_list[0], label_list[1]);
      leda_edge move_edge1, move_edge2;
      int source_label1, target_label1, source_label2, target_label2;
      leda_list<leda_edge> Temp_List;
      leda_list<leda_node> Move_Edge_List1;
      leda_list<leda_node> Move_Edge_List2;  
      if (s=='H')
      {
         move_edge1=triangle_matrix.get_edge(label_list[0], label_list[1]); 
         source_node=h_clo_G.source(move_edge1);
         target_node=h_clo_G.target(move_edge1);
         source_label1=h_clo_G[source_node];
         target_label1=h_clo_G[target_node];
         
         success=Rotate_Rectilinear(h_clo_G, v_clo_G, source_label1, target_label1, 'V', move_edge1, Move_Edge_List1);         
         if (success)
         { 
            Temporary_Move_Edges(v_clo_G, source_label1, target_label1, 'V', move_edge1, Move_Edge_List1, Temp_List);
            move_edge2=triangle_matrix.get_edge(label_list[1], label_list[2]); 
            source_node=h_clo_G.source(move_edge2);
            target_node=h_clo_G.target(move_edge2);
            source_label2=h_clo_G[source_node];
            target_label2=h_clo_G[target_node];
            success=Rotate_Rectilinear(h_clo_G, v_clo_G, source_label2, target_label2, 'V', move_edge2, Move_Edge_List2);
            if (success)
	    {
	       Recover_Move_Edges(v_clo_G, Temp_List, 'H');
	       Move_Edges(h_clo_G, v_clo_G, source_label1, target_label1, 'V', move_edge1, Move_Edge_List1);
	       Move_Edges(h_clo_G, v_clo_G, source_label2, target_label2, 'V', move_edge2, Move_Edge_List2);
	       for (int i=0;i<3;i++)
	       {
	          node_label=label_list[i];
                  rotate_module(node_label, width, height, 1);
                  h_clo_node=h_nodes[node_label];
                  v_clo_node=v_nodes[node_label];
                  assign_edges_length(h_clo_G, h_clo_node, width);
                  assign_edges_length(v_clo_G, v_clo_node, height);
 	       }
 	       return 1;           
            }
            else 
            {
               Recover_Move_Edges(v_clo_G, Temp_List, 'H');
               return 0;
            }
         }
         else
            return 0;
	 
      }
      else{
         move_edge1=triangle_matrix.get_edge(label_list[0], label_list[1]);
         source_node=v_clo_G.source(move_edge1);
         target_node=v_clo_G.target(move_edge1);
         source_label1=v_clo_G[source_node];
         target_label1=v_clo_G[target_node];
         success=Rotate_Rectilinear(v_clo_G, h_clo_G, source_label1, target_label1, 'H', move_edge1, Move_Edge_List1);
	 if (success)
	 {
	    Temporary_Move_Edges(h_clo_G, source_label1, target_label1, 'H', move_edge1, Move_Edge_List1, Temp_List);
	    move_edge2=triangle_matrix.get_edge(label_list[1], label_list[2]);
            source_node=v_clo_G.source(move_edge2);
            target_node=v_clo_G.target(move_edge2);
            source_label2=v_clo_G[source_node];
            target_label2=v_clo_G[target_node];
            success=Rotate_Rectilinear(v_clo_G, h_clo_G, source_label2, target_label2, 'H', move_edge2, Move_Edge_List2);
	    if (success)
	    {
	       Recover_Move_Edges(h_clo_G, Temp_List, 'V');
	       Move_Edges(v_clo_G, h_clo_G, source_label1, target_label1, 'H', move_edge1, Move_Edge_List1);
	       Move_Edges(v_clo_G, h_clo_G, source_label2, target_label2, 'H', move_edge2, Move_Edge_List2);
	       for (int i=0;i<3;i++)
	       {
	          node_label=label_list[i];
                  rotate_module(node_label, width, height, 0);
                  h_clo_node=h_nodes[node_label];
                  v_clo_node=v_nodes[node_label];
                  assign_edges_length(h_clo_G, h_clo_node, width);
                  assign_edges_length(v_clo_G, v_clo_node, height);
               }
               return 1;
            }
            else
            {
               Recover_Move_Edges(h_clo_G, Temp_List, 'V');
               return 0;
            }
         }
         else
            return 0;
      }//else in V

   }
}
//***************************************************************
//operator 4::interchang two modules 
//***************************************************************
void get_in_out_nodes(GRAPH<int, float>& G, leda_node& w1, 
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

void add_edges_cloG(GRAPH<int, float>& G, leda_node& w, leda_node& v,
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
   int sizeH=Num_of_Hard;
   int sizeL=Num_of_L;
   int count=0;

   node_label1=(rand()%size)+1;
   if (modules[node_label1-1].type == 0)
   {
      do{
         node_label2=(rand()%sizeH)+1;
         count++;
      }while(node_label1==node_label2 && count<3);
      //cout<<"(I) interchage Hard modules "<<node_label1+1<<" with node "<<node_label2+1<<endl;      
      Interchange_Rectilinear(node_label1, node_label2);
      
   }
   else if(modules[node_label1-1].type == 2)
   {
      int sub_label1=modules[node_label1-1].submodules[0].id+1;
      do{
         node_label2=(rand()%sizeL)+sizeH+1;
         count++;
      }while((node_label1==node_label2 || node_label2==sub_label1) && count<3);
      //cout<<"(I) interchage L modules "<<node_label1+1<<" with node "<<node_label2+1<<endl; 
      int sub_label2=modules[node_label2-1].submodules[0].id+1;	 

      Interchange_Rectilinear(node_label1, node_label2);
      Interchange_Rectilinear(sub_label1, sub_label2);
      
      int edge_type1=triangle_matrix.get_edge_type(node_label1, sub_label1);
      int edge_type2=triangle_matrix.get_edge_type(node_label2, sub_label2);
      
      if (edge_type1==0 || edge_type1==1)
      {
	 if (edge_type2==2 || edge_type2==3)
	 {
	    Rotate_Submodules(node_label1, 1);
	    Rotate_Submodules(sub_label1, 1);
	    Rotate_Submodules(node_label2, 0);
	    Rotate_Submodules(sub_label2, 0);
         }
        
      }
      else if (edge_type1==2 || edge_type1==3)
      {
      	 if (edge_type2==0 || edge_type2==1)
      	 {
 	    Rotate_Submodules(node_label1, 0);
	    Rotate_Submodules(sub_label1, 0);
	    Rotate_Submodules(node_label2, 1);
	    Rotate_Submodules(sub_label2, 1);     	 
         }
      }     	
   } //end type==2
   else if(modules[node_label1-1].type == 3)
   {
      int sub_label1=modules[node_label1-1].submodules[0].id+1;
      int sub_label2=modules[node_label1-1].submodules[1].id+1;
      int sizeT=size-sizeH-sizeL;
      do{
         node_label2=size-(rand()%sizeT);
         count++;
      }while((node_label1==node_label2 || node_label2==sub_label1 || node_label2==sub_label2) && count<3);
      //cout<<"(I) interchage T modules "<<node_label1+1<<" with node "<<node_label2+1<<endl; 
      int label_list1[3],label_list2[3];
      int index1=node_label1-1;
      if (index1<modules[index1].submodules[0].id)
      {
      	 label_list1[0]=index1+1;
      	 label_list1[1]=index1+2;
      	 label_list1[2]=index1+3;
      }
      else if (index1>modules[index1].submodules[1].id) 	
      {
      	 label_list1[0]=index1-1;
      	 label_list1[1]=index1;
      	 label_list1[2]=index1+1;
      }
      else
      {
      	 label_list1[0]=index1;
      	 label_list1[1]=index1+1;
      	 label_list1[2]=index1+2;
      }
      
      int index2=node_label2-1;
      if (index2<modules[index2].submodules[0].id)
      {
      	 label_list2[0]=index2+1;
      	 label_list2[1]=index2+2;
      	 label_list2[2]=index2+3;
      }
      else if (index2>modules[index2].submodules[1].id) 	
      {
      	 label_list2[0]=index2-1;
      	 label_list2[1]=index2;
      	 label_list2[2]=index2+1;
      }
      else
      {
      	 label_list2[0]=index2;
      	 label_list2[1]=index2+1;
      	 label_list2[2]=index2+2;
      }
      
      Interchange_Rectilinear(label_list1[0], label_list2[0]);
      Interchange_Rectilinear(label_list1[1], label_list2[1]);
      Interchange_Rectilinear(label_list1[2], label_list2[2]);
      
      int edge_type1=triangle_matrix.get_edge_type(label_list1[0], label_list1[1]);
      int edge_type2=triangle_matrix.get_edge_type(label_list2[0], label_list2[1]);
      
      if (edge_type1==0 || edge_type1==1)
      {
	 if (edge_type2==2 || edge_type2==3)
	 {
	    Rotate_Submodules(label_list1[0], 1);
	    Rotate_Submodules(label_list1[1], 1);
	    Rotate_Submodules(label_list1[2], 1);
	    Rotate_Submodules(label_list2[0], 0);
	    Rotate_Submodules(label_list2[1], 0);
	    Rotate_Submodules(label_list2[2], 0);
         }
        
      }
      else if (edge_type1==2 || edge_type1==3)
      {
      	 if (edge_type2==0 || edge_type2==1)
      	 {
	    Rotate_Submodules(label_list1[0], 0);
	    Rotate_Submodules(label_list1[1], 0);
	    Rotate_Submodules(label_list1[2], 0);
	    Rotate_Submodules(label_list2[0], 1);
	    Rotate_Submodules(label_list2[1], 1);
	    Rotate_Submodules(label_list2[2], 1);   	 
         }
      }      
   }
}

//***********************************************************************************
//Interchange two submodules of a rectilinear
//***********************************************************************************
void Clo_Red_Graph::Interchange_Rectilinear(int node_label1, int node_label2)
{
   leda_node h_red_node1, h_red_node2, v_red_node1, v_red_node2;
   leda_node h_clo_node1, h_clo_node2, v_clo_node1, v_clo_node2;

   h_clo_node1=h_nodes[node_label1];
   h_clo_node2=h_nodes[node_label2];
   v_clo_node1=v_nodes[node_label1];
   v_clo_node2=v_nodes[node_label2];
  
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
//***********************************************************************************
//Rotate submodules when inerchange two rectilinear modules
//***********************************************************************************
void Clo_Red_Graph::Rotate_Submodules(int node_label, int f)
{
   float width, height;
   leda_node h_clo_node, v_clo_node;
   rotate_module(node_label, width, height, f);
   h_clo_node=h_nodes[node_label];
   v_clo_node=v_nodes[node_label];
   assign_edges_length(h_clo_G, h_clo_node, width);
   assign_edges_length(v_clo_G, v_clo_node, height);
}

//***************************************************************
////operator 5::move an edge from one graph to another and reverse
//***************************************************************
int Clo_Red_Graph::move_edge_reverse(GRAPH<int, float>& CG_from,
                             GRAPH<int, float>& CG_to, char d)
{
   leda_edge move_edge;
   leda_node source_node, target_node, pick_node;
   int source_label, target_label;

   int success=0,count=0;

   while (success==0 && count<2)
   {
      pick_node=CG_from.choose_node();
      while (pick_node==CG_from.first_node() || pick_node==CG_from.last_node())
         pick_node=CG_from.choose_node();
      success=Pick_Red_Edge(CG_from, move_edge, pick_node);
      count++;
   }

   if(success){
     //extract the corresponding source (target) nodes from another
     //closure graph

     //cout<<"(M) change edge==>"<<"<"<<source_label+1<<", "<<target_label+1<<">"<<endl;

     source_node=CG_from.source(move_edge);
     target_node=CG_from.target(move_edge);
     source_label=CG_from[source_node];
     target_label=CG_from[target_node];

     if(d=='H'){
       source_node=h_nodes[source_label];
       target_node=h_nodes[target_label];
     } 
     else{
       source_node=v_nodes[source_label];
       target_node=v_nodes[target_label];
     }
     //cout<<"start move&reverse edge "<<source_label+1<<" "<<target_label+1<<"  destination "<<d<<endl; 
     leda_list<leda_node> Move_Edge_List;
     int OK=Compute_Move_Edges(CG_to, CG_from, target_node, source_node, Move_Edge_List, d);
     if(OK==1)
     {
     	Tune_Graph_when_Add(CG_to, target_node, source_node, modules_info, CG_from, d, triangle_matrix);
     	int s_label, t_label;
     	float edge_length;
     	list_item litem, Sitem, Titem;
     	leda_edge e;
     	int i=0;	 
      	forall_items(litem, Move_Edge_List)
      	{  
          if(i%2==0){
            Sitem=litem; 
          }
          else {
            Titem=litem; 
            s_label=CG_to[Move_Edge_List[Sitem]];
            t_label=CG_to[Move_Edge_List[Titem]];
            if(d=='H')
              edge_length=modules_info[s_label-1].width;
            else
              edge_length=modules_info[s_label-1].height;
      
            e=CG_to.new_edge(Move_Edge_List[Sitem], Move_Edge_List[Titem]);
            CG_to.assign(e, edge_length);
            triangle_matrix.Update_Matrix(s_label, t_label, d, CG_from, e);
          }
          i++; 		
        }
        return 1;
     }
     else
        return 0;

     return 1;
   }
   else
     return 0;
}

//***********************************************************************************
//Operator : Flipping a rectilinear module
//***********************************************************************************
void Clo_Red_Graph::Flipping_Rectilinear(char d)
{
   int node_label1,node_label2;
   int size=modules_N;
   int sizeH=Num_of_Hard;
   int index;
   
   do{
      node_label1=(rand()%(size-sizeH))+1+sizeH;      
   }while(modules[node_label1-1].type == 0);
   
   //cout<<d<<"  Flipping_Rectilinear module label --> "<<node_label1+1<<endl;
   index=node_label1-1;
   if (d=='H')
   {
      if (modules[index].type == 2)
      {
         node_label2=modules[index].submodules[0].id+1;
         Interchange_Rectilinear(node_label1,node_label2);
      }
      else
      {
         if (index<modules[index].submodules[0].id)
         {
      	    node_label1=index+1;
      	    node_label2=index+3;
         }
         else if (index>modules[index].submodules[1].id) 	
         {
      	    node_label1=index-1;
      	    node_label2=index+1;
         }
         else
         {
      	    node_label1=index;
      	    node_label2=index+2;
         }
         Interchange_Rectilinear(node_label1,node_label2);    
      }
   }
   else
   { 
      if (modules[index].type == 2)
      {
         Revese_relation(index);
         Revese_relation(modules[index].submodules[0].id);
      }
      else
      {
      	 Revese_relation(index);
         Revese_relation(modules[index].submodules[0].id);
	 Revese_relation(modules[index].submodules[1].id);
      }
   }

}

//***************************************************************
//permutate
//***************************************************************
void Clo_Red_Graph::permutate()
{
   float rotate_rate=0.6;
   int success=1;
   int c;
   int count=0;
   do{  
         c=rand()%2;
                if(c){
                  //cout<<"move edges from h graph to v graph"<<endl;
                  success=move_edge(h_clo_G, v_clo_G, 'V');
                }else{
                  //cout<<"move edges from v graph to h graph"<<endl;
                  success=move_edge(v_clo_G, h_clo_G, 'H'); 
                }
       count++;
   }while(!success && count<10);
   //show_modules();
}

void Clo_Red_Graph::permutate_1()
{
   int success=1;
   int c;
   float rotate_rate=0.6;
   do{ 
      if(rotate_rate>rand_01()){          
      	c=rand()%3;
      	switch(c)
      	{
      	     case 0:{  
                c=rand()%2;
                if(c)
                {  //cout<<"move edges from h graph to v graph"<<endl;
                   success=move_edge(h_clo_G, v_clo_G, 'V');
                }
                else
                {  //cout<<"move edges from v graph to h graph"<<endl;
                   success=move_edge(v_clo_G, h_clo_G, 'H'); 
                }
                break;} 
             case 1:{   
                c=rand()%2;
                if(c)
                {  //cout<<"reverse edge in h graph"<<endl;
                   success=reverse_edge(h_clo_G, v_clo_G, 'H');
                }
                else
                {  //cout<<"reverse edge in v graph"<<endl;
                   success=reverse_edge(v_clo_G, h_clo_G, 'V');
                }
                break;}
             case 2:{   
                c=rand()%2;
                if(c)
                {  //cout<<"move edges from h graph to v graph and reverse"<<endl;
                   success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                }
                else
                {
                   //cout<<"move edges from v graph to h graph and reverse"<<endl;
                   success=move_edge_reverse(v_clo_G, h_clo_G, 'H'); 
                }
                break;} 
	  }//end of switch
      }//end if 
      else{
         c=rand()%3;
         switch (c) 
         {
           case 0:{  //cout<<"interchange two nodes"<<endl;
              interchange();
              break;}
           case 1:{  //Flip a rectlinear modules;
              c=rand()%2;
              if(c)
              {  //cout<<"Perpendicular_Flipping a rectlinear modules"<<endl;
                 Flipping_Rectilinear('V');
              }
              else
              {  //cout<<"Parallel_Flipping a rectlinear modules"<<endl;
                 Flipping_Rectilinear('H'); 
              }
              break;}                   
           case 2:{
            c=rand()%2; 
            	//cout<<"rotate the direction of an module in positive degree"<<endl;
            success=rotate(c);
            break;}
         }//end of switch
      } // end of else
   }while(!success);
}

void Clo_Red_Graph::permutate_2()
{
   int success=1;
   float rotate_rate=0.25;
   do{ 
      if(rotate_rate>rand_01()){          
         int c1=rand()%2;
         if(c1)
         {  //cout<<"rotate the direction of an module in positive degree"<<endl;
            success=rotate(c1);
         }
         else
         {  //cout<<"rotate the direction of an module in negative degree"<<endl;
            success=rotate(c1);
         }
      } 
      else{
         int c=rand()%5;
	 //int c=0;
         switch (c) {
           case 0:{ //cout<<"interchange two nodes"<<endl;
              interchange();
              break;}
           case 1:{ 
              c=rand()%2;
              if(c)
              {  //cout<<"move edges from h graph to v graph"<<endl;
                 success=move_edge(h_clo_G, v_clo_G, 'V');
              }
              else
              {  //cout<<"move edges from v graph to h graph"<<endl;
                 success=move_edge(v_clo_G, h_clo_G, 'H'); 
              }
              break;} 
           case 2:{
              c=rand()%2;
              if(c)
              {  //cout<<"reverse edge in h graph"<<endl;
                 success=reverse_edge(h_clo_G, v_clo_G, 'H');
              }
              else
              {  //cout<<"reverse edge in v graph"<<endl;
                 success=reverse_edge(v_clo_G, h_clo_G, 'V');
              }
              break;}
           case 3:{
              c=rand()%2;
              if(c)
              {  //cout<<"move edges from h graph to v graph and reverse"<<endl;
                 success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
              }
              else
              {  //cout<<"move edges from v graph to h graph and reverse"<<endl;
                 success=move_edge_reverse(v_clo_G, h_clo_G, 'H'); 
              }
              break;} 
           case 4:{
              c=rand()%2;
              if(c)
              {  //cout<<"Perpendicular_Flipping a rectlinear modules"<<endl;
                 Flipping_Rectilinear('V');
              }
              else
              {  //cout<<"Parallel_Flipping a rectlinear modules"<<endl;
                 Flipping_Rectilinear('H'); 
              }
              break;}           
         }//end of switch
      }// end else 
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

void Clo_Red_Graph::recover(Solution &sol){
  copy_graph(sol.h_clo_G, h_clo_G, triangle_matrix, 'H', h_nodes);
  copy_graph(sol.v_clo_G, v_clo_G, triangle_matrix, 'V', v_nodes);
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
              packing();
              show_graph(getWidth(), getHeight(), modules_info);
              exit(0);
            }
       }
      
       float length=h_clo_G[e];
       if(source_label==0){
          if(length!=0 && target_label!=modules_N+1){
             cout<<"Length Error of h_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!  length="<<length<<endl;
             list_information();
             packing();
             show_graph(getWidth(), getHeight(), modules_info);
             exit(0);
          }
       }else{
          if(length!=modules_info[source_label-1].width && target_label!=modules_N+1){
             cout<<"Length Error of h_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!  length="<<length<<endl;
             list_information();
             packing();
             show_graph(getWidth(), getHeight(), modules_info);
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
               packing();
               show_graph(getWidth(), getHeight(), modules_info);
               exit(0);
            }
       }

       float length=v_clo_G[e];
       if(source_label==0){
          if(length!=0 && target_label!=modules_N+1){
             cout<<"Length Error of v_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!  length="<<length<<endl;
             list_information();
             packing();
             show_graph(getWidth(), getHeight(), modules_info);
             exit(0);
          }
       }else{
          if(length!=modules_info[source_label-1].height && target_label!=modules_N+1){
             cout<<"Length Error of v_clo_G edge <"<< source_label+1
                 <<", "<<target_label+1<<">!!!  length="<<length<<endl;
             list_information();
             packing();
             show_graph(getWidth(), getHeight(), modules_info);
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

//***********************************************************************************
//Operator : Rotate a rectilinear module
//***********************************************************************************
int Clo_Red_Graph::Rotate_Rectilinear(GRAPH<int, float>& CG_from, GRAPH<int, float>& CG_to, 
                                   int source_label, int target_label ,char d, leda_edge move_edge, leda_list<leda_node>& Move_Edge_List)
{
   //leda_edge move_edge;
   leda_node source_node, target_node;
   if(d=='H'){
      source_node=h_nodes[source_label];
      target_node=h_nodes[target_label];
   }
   else {
      source_node=v_nodes[source_label];
      target_node=v_nodes[target_label];
   }
     
   int OK=Compute_Move_Edges(CG_to, CG_from, target_node, source_node, Move_Edge_List, d);
   if (OK)
      return 1;
   else 
      return 0;
    
}

//***********************************************************************************
//Move_Edges when rotating a rectilinear module
//***********************************************************************************
void Clo_Red_Graph::Move_Edges(GRAPH<int, float>& CG_from, GRAPH<int, float>& CG_to, 
                                   int source_label, int target_label ,char d, leda_edge move_edge, leda_list<leda_node>& Move_Edge_List)
{     
    leda_node source_node, target_node; 
    if(d=='H'){
       source_node=h_nodes[source_label];
       target_node=h_nodes[target_label];
    }
    else {
       source_node=v_nodes[source_label];
       target_node=v_nodes[target_label];
    }
    Tune_Graph_when_Add(CG_to, target_node, source_node, modules_info, CG_from, d, triangle_matrix);
    //cout<<"Move edge "<<target_label+1<<" "<<source_label+1<<endl;
    int s_label, t_label;
    float edge_length;
    list_item litem, Sitem, Titem;
    leda_edge e;
    int i=0;	 
    forall_items(litem, Move_Edge_List)
    {  
       if(i%2==0){
          Sitem=litem; 
       }
       else {
          Titem=litem; 
          s_label=CG_to[Move_Edge_List[Sitem]];
          t_label=CG_to[Move_Edge_List[Titem]];
          //cout<<"Move edge "<<s_label+1<<" "<<t_label+1<<endl;
          if(d=='H')
             edge_length=modules_info[s_label-1].width;
          else
             edge_length=modules_info[s_label-1].height;
      
          e=CG_to.new_edge(Move_Edge_List[Sitem], Move_Edge_List[Titem]);
	  //cout<<"Move edge "<<CG_to[CG_to.source(e)]+1<<" "<<CG_to[CG_to.target(e)]+1<<endl;
          CG_to.assign(e, edge_length);
          triangle_matrix.Update_Matrix(s_label, t_label, d, CG_from, e);
          
       }
       i++; 		
    }    
}

//***********************************************************************************
//Pre-Move_Edges when rotating a T-shaped module
//***********************************************************************************

void Clo_Red_Graph::Temporary_Move_Edges(GRAPH<int, float>& CG_to, int source_label, int target_label, char d, 
				leda_edge move_edge, leda_list<leda_node>& Move_Edge_List, leda_list<leda_edge>& Temp_List)
{     
    leda_node source_node, target_node; 
    if(d=='H'){
       source_node=h_nodes[source_label];
       target_node=h_nodes[target_label];
    }
    else {
       source_node=v_nodes[source_label];
       target_node=v_nodes[target_label];
    }
    leda_edge e;
    e=CG_to.new_edge(target_node, source_node);
    triangle_matrix.set_edge_type(target_label, source_label, d);
    //cout<<"Temporary Move edge "<<CG_to[CG_to.source(e)]+1<<" "<<CG_to[CG_to.target(e)]+1<<endl;
    Temp_List.append(e);
    int s_label, t_label;
    list_item litem, Sitem, Titem;
    int i=0;	 
    forall_items(litem, Move_Edge_List)
    {  
       if(i%2==0){
          Sitem=litem; 
       }
       else {
          Titem=litem; 
          s_label=CG_to[Move_Edge_List[Sitem]];
          t_label=CG_to[Move_Edge_List[Titem]];
          //cout<<"Tempporary Move edge "<<s_label+1<<" "<<t_label+1<<endl;
          e=CG_to.new_edge(Move_Edge_List[Sitem], Move_Edge_List[Titem]);
          triangle_matrix.set_edge_type(s_label, t_label, d);
          //cout<<"Temporary Move edge "<<CG_to[CG_to.source(e)]+1<<" "<<CG_to[CG_to.target(e)]+1<<endl;
          Temp_List.append(e);
       }
       i++; 		
    }    
}

void Clo_Red_Graph::Recover_Move_Edges(GRAPH<int, float>& CG_to, leda_list<leda_edge>& Temp_List, char d)
{     
    list_item litem;
    leda_edge e;	
    int source_label,target_label; 
    forall_items(litem, Temp_List)
    {  
	e=Temp_List[litem];
	source_label=CG_to[CG_to.source(e)];
	target_label=CG_to[CG_to.target(e)];
	triangle_matrix.set_edge_type(target_label, source_label, d);
	//cout<<"Recover Move edge "<<CG_to[CG_to.source(e)]+1<<" "<<CG_to[CG_to.target(e)]+1<<endl;
	CG_to.del_edge(e);	
    }    
}

//***********************************************************************************
//Debug
//***********************************************************************************
void Clo_Red_Graph::Debug()
{
   leda_edge e1, e2;
   leda_node first_node,last_node;
   int Edge_count=0;
   int count=0;
   first_node=h_clo_G.first_node();
   last_node=h_clo_G.last_node();
   forall_edges(e1, h_clo_G)
   {
      if (h_clo_G.source(e1)!=first_node && h_clo_G.target(e1)!=last_node)
      	 Edge_count++;
      else if (h_clo_G.source(e1)==h_clo_G.target(e1))
      {
         count++;
         //cout<<"H "<<h_clo_G[h_clo_G.source(e1)]+1<<endl;
      }
   }    
   
   first_node=v_clo_G.first_node();
   last_node=v_clo_G.last_node();
   forall_edges(e1, v_clo_G)
   {
      if (v_clo_G.source(e1)!=first_node && v_clo_G.target(e1)!=last_node)
      	 Edge_count++;
      else if (v_clo_G.source(e1)==v_clo_G.target(e1))
      {
         count++;
         //cout<<"V "<<v_clo_G[v_clo_G.source(e1)]+1<<endl;
      }
   }    
   //cout<<"count="<<count<<endl;
   //cout<<"Edge_count="<<Edge_count<<endl;
   if (modules_N*(modules_N-1)/2 != Edge_count)
   {
   	cout<<endl<<"Edge_count = "<<Edge_count<<endl; 
   	show_graph(getWidth(), getHeight(), modules_info);
   	exit(0);
   }      
   
   if (getArea()<TotalArea)
   {
   	cout<<"TotalArea="<<TotalArea<<endl;
   	cout<<"Area="<<getArea()<<"  GAME OVER!!!"<<endl;
   	show_graph(getWidth(), getHeight(), modules_info);
   	exit(0);
   }
   
}
