#include "top_sort.h"
//-----------------------------------------------------------
void DFS_VISIT(GRAPH<int, int> &G, leda_node w, leda_node_array<int>& colors, 
               leda_node_array<int>& top_ord, int& count)
{   
   leda_node u;
   leda_list<leda_node> adj_nodes;

   colors[w]=1;
   adj_nodes=G.adj_nodes(w);
   list_item litem;

   forall_items(litem, adj_nodes)
   {
      u=adj_nodes[litem];

      if(colors[u]==0)
      {
        //cout<<"node "<<G[u]<<endl;
        DFS_VISIT(G, u, colors, top_ord, count);
      }
   }    
   colors[w]=2;  
   top_ord[w]=count--;
   //cout<<"node" <<G[w]<<"="<<top_ord[w]<<endl;
}

void DFS(GRAPH<int, int> &G, leda_node_array<int>& top_ord)
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
         //cout<<"node "<<G[w]<<endl;
         DFS_VISIT(G, w, colors, top_ord, count);
      }
   }  
}

void Topologic_Sort(GRAPH<int, int> &G, leda_node_array<int>& top_ord)
{
   DFS(G, top_ord);
}