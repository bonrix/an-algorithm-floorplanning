#include "top_sort.h"
#include "fplan.h"
//-----------------------------------------------------------
void DFS_VISIT(GRAPH<int, float> &G, leda_node w, leda_node_array<int>& colors, 
               leda_node_array<int>& top_ord, int& count)
{   
   leda_node u;
   leda_list<leda_node> adj_nodes;

   colors[w]=1;
   adj_nodes=G.adj_nodes(w);
   list_item litem;
//   cout<<"visited node "<<G[w]+1<<endl;

   forall_items(litem, adj_nodes)
   {
      u=adj_nodes[litem];

      if(colors[u]==0)
      {
//        cout<<"adj node "<<G[u]+1<<endl;
        DFS_VISIT(G, u, colors, top_ord, count);
      }
   }    
   colors[w]=2;  
   top_ord[w]=count--;
}

void DFS(GRAPH<int, float> &G, leda_node_array<int>& top_ord)
{
   int n=G.number_of_nodes();
   leda_node_array<int> colors(G); //'0' denote white
                                   //'1' denote graph
                                   //'2' denote black
   leda_node w;
   int count=n;
 
   forall_nodes(w, G)
      colors[w]=0;
   
   forall_nodes(w, G)
   {
      if(colors[w]==0)
      {
        
         DFS_VISIT(G, w, colors, top_ord, count);
      }
   }  
}

void BFS(GRAPH<int, float>& G, Modules& modules, leda_list<leda_edge>& Addtional_edges, vector<leda_node>& G_nodes, char d, Modules_Info& modules_info, Hv_Matrix& triangle_matrix)
{
   int n=G.number_of_nodes();
   leda_node_array<int> colors(G); //'0' denote white
                                   //'1' denote graph
                                   //'2' denote black
   leda_node_array<int> Access(G); //'1' denote accessed
   				   //'0' denote didn't accessed
   leda_list<leda_node> Submodules; 				   
   leda_list<leda_node> Queue;
   leda_list<leda_node> adj_nodes;
   leda_edge e;
   leda_node w,u,v;
   list_item litem;
   int count=n;
   int node_index;	
   forall_nodes(w, G)
   {
      colors[w]=0;
      Access[w]=1;
   }
   
   Queue.append(G.first_node());
   colors[G.first_node()]=1;
   
   while (Queue.size()!=0)
   {
      u=Queue.pop();
      adj_nodes=G.adj_nodes(u);
      Submodules.clear();
      forall_items(litem, adj_nodes)
      {
         v=adj_nodes[litem];
         if(colors[v]==0)
         {
            //cout<<"First visit node "<<G[v]+1<<endl;
            colors[v]=1;
            Queue.append(v);
         }

         node_index=G[v]-1;
         if(u!=G.first_node() && v!=G.last_node())
         {
            if(modules[node_index].type!=0 && triangle_matrix.get_edge_type_c(node_index+1, modules[node_index].submodules[0].id+1)!=d)
            {

               if (modules[node_index].type==2)
               {
                  if (Access[v]==1)
                  {
                     w=G_nodes[modules[node_index].submodules[0].id+1];
                     Access[w]=0;             
                     Submodules.append(w);
                  } 	
                  else
                  {
           	     Submodules.remove(v);
                     Access[v]=1;
                  }
               }//if=2
               else if (modules[node_index].type==3)
               {
                  //cout<<"BFS"<<endl;
                  if (Access[v]==1)
                  {
                     w=G_nodes[modules[node_index].submodules[0].id+1];
                     Access[w]=0;
                     Submodules.append(w);                     
                     w=G_nodes[modules[node_index].submodules[1].id+1];
                     Access[w]=0;
                     Submodules.append(w);
                  } 	
                  else
                  {
                     Submodules.remove(v);
                     Access[v]=1;
                  }	
               }//if==3
             }//if needs add edges  
         }//ifu!=first_node v!=last_node
      }//forall_itme    
      colors[u]=2;
      
      if (u!=G.first_node()&& u!=G.last_node())
      {  
         forall_items(litem, Submodules)
         {
      	    w=Submodules[litem];
      	    e=G.new_edge(u,w);
      	    Addtional_edges.append(e);
	    Access[w]=1;
         }
      }	      
   }//end while
}

void Topologic_Sort(GRAPH<int, float>& G, leda_node_array<int>& top_ord)
{
   DFS(G, top_ord);
}

void Packing_Topologic_Sort(GRAPH<int, float>& G, Modules& modules, leda_node_array<int>& top_ord, vector<leda_node>& G_nodes, char d, Modules_Info& modules_info, Hv_Matrix& triangle_matrix)
{
   leda_list<leda_edge> Addtional_edges;
   BFS(G, modules, Addtional_edges, G_nodes, d, modules_info, triangle_matrix);
   DFS(G, top_ord);
   list_item litem;
   leda_edge e;

   leda_node n;

   forall_items(litem, Addtional_edges)
   {
      e=Addtional_edges[litem];
      G.del_edge(e);
   }	
}