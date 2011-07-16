#include <LEDA/array.h>
#include <LEDA/graph_iterator.h>
#include <LEDA/list.h>
#include <vector>
#include <math.h>
#include <time.h>
#include "clo_red.h"
//---------------------------------------------------------------
// Initialization CGS
//---------------------------------------------------------------
void Clo_Red_Graph::init()
{
   int i, j;
   leda_node n;
   leda_edge e;
   triangle_matrix.init(modules_N);
   list_item litem;
   Snode sn;

   for(i=0; i <modules_N; i++){
      //construct nodes for HCG
      n=h_clo_G.new_node();
      h_nodes.push_back(n);
      h_clo_G.assign(n, i);

      //construct nodes for VCG
      n=v_clo_G.new_node();
      v_nodes.push_back(n);
      v_clo_G.assign(n, i);

      //for packing sequence
      sn.label=i;
      sn.pt=i;
      litem=packing_seq.append(sn);
       
      packing_litems.push_back(litem);  
   }

   for(i=0; i <modules_N; i++){
      //construct edges for HCG
      for(j=i+1; j< modules_N; j++){
         e=h_clo_G.new_edge(h_nodes[i], h_nodes[j]);

         triangle_matrix.set_edge(i, j, e);
         triangle_matrix.set_edge_type(i, j, H);
      }

      //for vertical graph
   }
   //list_information;
}

void Clo_Red_Graph::init_sqr()
{
   int i, j;
   leda_node n;
   leda_edge e;
   triangle_matrix.init(modules_N);
   list_item litem;
   Snode sn;
 

   for(i=0; i <modules_N; i++){
      //construct nodes for HCG
      n=h_clo_G.new_node();
      h_nodes.push_back(n);
      h_clo_G.assign(n, i);

      //construct nodes for VCG
      n=v_clo_G.new_node();
      v_nodes.push_back(n);
      v_clo_G.assign(n, i);

      //for packing sequence
      sn.label=i;
      sn.pt=i;
      litem=packing_seq.append(sn);
       
      packing_litems.push_back(litem);  
   }
   
   int k, l=1;
   int len=(int)ceil(sqrt(modules_N));
   for(i=0; i <modules_N; i++){
      l=(i%len)+1; 
      for(j=i+1; j< modules_N; j++){     
         if(l<len){ 
           e=h_clo_G.new_edge(h_nodes[i], h_nodes[j]);

           triangle_matrix.set_edge(i, j, e);
           triangle_matrix.set_edge_type(i, j, H);
         }else{
           e=v_clo_G.new_edge(v_nodes[i], v_nodes[j]);

           triangle_matrix.set_edge(i, j, e);
           triangle_matrix.set_edge_type(i, j, V);
         }
         l++; 
      }
   }
   //list_information;
}

//---------------------------------------------------------------
// Packing Algorithm using Contour Packing 
//---------------------------------------------------------------
void Clo_Red_Graph::contour_packing(){
    list_item H_litem, V_litem, H_pre_litem, V_pre_litem, cur_litem, litem;
    Snode init_sn, tmp_sn, old_sn, sn;
    Snode H_pre_sn, V_pre_sn;
    leda_list<Snode> H_contour, V_contour;
    char direction;
    int last_node;
    int i;
    vector<int> H_flags, V_flags;
    vector<list_item> H_litems, V_litems;
    H_flags.resize(modules_N);
    V_flags.resize(modules_N);
    H_litems.resize(modules_N);
    V_litems.resize(modules_N);

    for(i=0; i < modules_N; i++){
       H_flags[i]=1;   
       V_flags[i]=1;
    }

    //initialize H_contour and V_contour
    //"-2" denote first node
    sn.label=-2;
    H_contour.append(sn);
    V_contour.append(sn);
    init_sn=packing_seq.pop();
    modules_info[init_sn.label].x=0;
    modules_info[init_sn.label].rx=modules_info[init_sn.label].width;
    modules_info[init_sn.label].y=0;
    modules_info[init_sn.label].ry=modules_info[init_sn.label].height;
    H_pre_litem=H_contour.append(init_sn);
    H_litems[init_sn.label]=H_pre_litem;
    V_pre_litem=V_contour.append(init_sn);
    V_litems[init_sn.label]=V_pre_litem;
    old_sn=init_sn;
    //"-1" denote last node
    sn.label=-1;
    H_contour.append(sn);
    V_contour.append(sn);

    //packing according to the modules on contour
    forall(sn, packing_seq){
       direction=triangle_matrix.get_edge_type_c(old_sn.pt, sn.pt); 
      
       //horizontal relation betwenn the current node
       //and previous node 
       if(direction=='H'){
         //compute x coordinate of H contour
         H_litem=H_contour.succ(H_pre_litem);      
         tmp_sn=H_contour[H_litem]; 
         
         do{//non-end node
             if(tmp_sn.label==-1){
               last_node=-1;
               H_pre_sn=H_contour[H_contour.pred(H_litem)];
               modules_info[sn.label].x=modules_info[H_pre_sn.label].rx;             
               modules_info[sn.label].rx=modules_info[sn.label].x+
                                     modules_info[sn.label].width;

               //insert new module into the contour
               H_pre_litem=H_contour.insert(sn, H_litem, LEDA::before);
               H_litems[sn.label]=H_pre_litem;
               break;
             }

             direction=triangle_matrix.get_edge_type_c(tmp_sn.pt, sn.pt);

             if(direction=='H'){ 
               H_litem=H_contour.succ(H_litem);
               tmp_sn=H_contour[H_litem];
             }else if(direction=='V'){
               H_pre_sn=H_contour[H_contour.pred(H_litem)];
               modules_info[sn.label].x=modules_info[H_pre_sn.label].rx;             
               modules_info[sn.label].rx=modules_info[sn.label].x
                                      +modules_info[sn.label].width;
  
               //find the node that is not deleted in V graph
               list_item tmp_litem=H_litem;
               last_node=tmp_sn.label;
               do{
                  if(V_flags[last_node]){
                    break;   
                  }
                  tmp_litem=H_contour.succ(tmp_litem);
                  last_node=H_contour[tmp_litem].label;
               }while(1);
  
               //insert new module into the contour
               H_pre_litem=H_contour.insert(sn, H_litem, LEDA::before);
               H_litems[sn.label]=H_pre_litem;

               //delete those succ nodes whose x coordinate smaller
               //or equal to current node
               cur_litem=H_litem; 
               while(tmp_sn.label!=-1&&
                     modules_info[tmp_sn.label].rx<=modules_info[sn.label].rx){
                  H_flags[tmp_sn.label]=0;
                  H_litem=H_contour.succ(H_litem);
                  tmp_sn=H_contour[H_litem];
                  H_contour.del(cur_litem);
                  cur_litem=H_litem;      
               }
               break;            
             }else{
               cout<<"Error of contour_packing in file clo_red.cc"<<endl;
               exit(0);                   
             }                 
         }while(1);                

          
         //compute y coordinate of V contour
         if(last_node!=-1)
           V_pre_litem=V_litems[last_node];
         else
           V_pre_litem=V_contour.last();
        
         V_litem=V_contour.pred(V_pre_litem);      
         tmp_sn=V_contour[V_litem]; 
       
         do{//non-source node
             if(tmp_sn.label==-2){
               //extract the succ node
               V_pre_sn=V_contour[V_contour.succ(V_litem)];
               modules_info[sn.label].y=modules_info[V_pre_sn.label].ry;             
               modules_info[sn.label].ry=modules_info[sn.label].y+
                                         modules_info[sn.label].height;

               //insert new module into the contour
               V_pre_litem=V_contour.insert(sn, V_litem, LEDA::after);
               V_litems[sn.label]=V_pre_litem;   
    
               //delete those pre nodes whose y coordinates smaller
               //or equal to current node
               V_litem=V_contour.pred(V_pre_litem);
               tmp_sn=V_contour[V_litem];
               cur_litem=V_litem;

               while(tmp_sn.label!=-2&&
                     modules_info[tmp_sn.label].ry<=modules_info[sn.label].ry){
                   V_flags[tmp_sn.label]=0; 
                   V_litem=V_contour.pred(V_litem);
                   tmp_sn=V_contour[V_litem];
                   V_contour.del(cur_litem);
                   cur_litem=V_litem;
               }
               break;
             }

             direction=triangle_matrix.get_edge_type_c(tmp_sn.pt, sn.pt);

             if(direction=='V'){ 
               V_litem=V_contour.pred(V_litem);
               tmp_sn=V_contour[V_litem];
             }else if(direction=='H'){
               V_pre_sn=V_contour[V_contour.succ(V_litem)];
  
               //end node
               if(V_pre_sn.label==-1){
                  modules_info[sn.label].y=0;             
                  modules_info[sn.label].ry=modules_info[sn.label].height;
               }else{
                  modules_info[sn.label].y=modules_info[V_pre_sn.label].ry;             
                  modules_info[sn.label].ry=modules_info[sn.label].y
                                        +modules_info[sn.label].height;
               }                 

               //insert new module into the contour
               V_pre_litem=V_contour.insert(sn, V_litem, LEDA::after);
               V_litems[sn.label]=V_pre_litem;          
   
               //delete those pred nodes whose y coordinates smaller
               //or equal to current node
               V_litem=V_contour.pred(V_pre_litem);
               tmp_sn=V_contour[V_litem];  
               cur_litem=V_litem; 
               while(tmp_sn.label!=-2&& //non-source module
                     modules_info[tmp_sn.label].ry<=modules_info[sn.label].ry){
                  V_flags[tmp_sn.label]=0; 
                  V_litem=V_contour.pred(V_litem);
                  tmp_sn=V_contour[V_litem];
                  V_contour.del(cur_litem);
                  cur_litem=V_litem;      
               }
               break;            
             }else{
               cout<<"Error of contour_packing in file clo_red.cc"<<endl;
               exit(0);                   
             }                 
         }while(1);                
       //vertical edge
       }else if(direction=='V'){ 
         //compute y coordinate in V contour
         V_litem=V_contour.pred(V_pre_litem);      
         tmp_sn=V_contour[V_litem]; 

         do{//non-source node
             if(tmp_sn.label==-2){
                last_node=-2;
                V_pre_sn=V_contour[V_contour.succ(V_litem)];
                modules_info[sn.label].y=modules_info[V_pre_sn.label].ry;             
                modules_info[sn.label].ry=modules_info[sn.label].y
                                         +modules_info[sn.label].height;

                //insert new module into the contour
                V_pre_litem=V_contour.insert(sn, V_litem, LEDA::after);
                V_litems[sn.label]=V_pre_litem;
                break;  
             }
             direction=triangle_matrix.get_edge_type_c(tmp_sn.pt, sn.pt);

             if(direction=='V'){ 
               V_litem=V_contour.pred(V_litem);
               tmp_sn=V_contour[V_litem];
             }else if(direction=='H'){
               V_pre_sn=V_contour[V_contour.succ(V_litem)];
               modules_info[sn.label].y=modules_info[V_pre_sn.label].ry;             
               modules_info[sn.label].ry=modules_info[sn.label].y
                                        +modules_info[sn.label].height;
 
               //find the node that is not deleted in V graph
               list_item tmp_litem=V_litem;
               last_node=tmp_sn.label;
               do{
                  if(H_flags[last_node]){
                    break;   
                  }
                  tmp_litem=V_contour.pred(tmp_litem);
                  last_node=V_contour[tmp_litem].label;
               }while(1);

               //insert new module into the contour
               V_pre_litem=V_contour.insert(sn, V_litem, LEDA::after);
               V_litems[sn.label]=V_pre_litem; 
             
               //delete those pre nodes whose y coordinate smaller
               //or equal to current node
               cur_litem=V_litem;
               while(tmp_sn.label!=-2&&
                      modules_info[tmp_sn.label].ry <= 
                      modules_info[sn.label].ry){
                  V_flags[tmp_sn.label]=0;
                  V_litem=V_contour.pred(V_litem);
                  tmp_sn=V_contour[V_litem];
                  V_contour.del(cur_litem);
                  cur_litem=V_litem;
               } 
               break;            
             }else{
               cout<<"Error of contour_packing in file clo_red.cc"<<endl;
               exit(0);                   
             }                 
         }while(1);                 

        //compute x coordinate in H contour
         if(last_node!=-2)
            H_pre_litem=H_litems[last_node];
         else
            H_pre_litem=H_contour.first();

         H_litem=H_contour.succ(H_pre_litem);      
         tmp_sn=H_contour[H_litem]; 
         
         do{//non-end node
             if(tmp_sn.label==-1){
                H_pre_sn=H_contour[H_contour.pred(H_litem)];
                modules_info[sn.label].x=modules_info[H_pre_sn.label].rx;
                modules_info[sn.label].rx=modules_info[sn.label].x+
                                          modules_info[sn.label].width;

                //insert new module into the contour
                H_pre_litem=H_contour.insert(sn, H_litem, LEDA::before);
                H_litems[sn.label]=H_pre_litem;
                 
                //delete those succ nodes whose x coordinate smaller
                //or equal to current node
                H_litem=H_contour.succ(H_pre_litem);
                tmp_sn=H_contour[H_litem];
                cur_litem=H_litem;
                while(tmp_sn.label!=-1&&
                    modules_info[tmp_sn.label].rx<=modules_info[sn.label].rx){
                  H_flags[tmp_sn.label]=0;
                  H_litem=H_contour.succ(H_litem);
                  tmp_sn=H_contour[H_litem];
                  H_contour.del(cur_litem);
                  cur_litem=H_litem;
                }
                break;
             }
             
             direction=triangle_matrix.get_edge_type_c(tmp_sn.pt, sn.pt);

             if(direction=='H'){ 
               H_litem=H_contour.succ(H_litem);
               tmp_sn=H_contour[H_litem];
             }else if(direction=='V'){
               H_pre_sn=H_contour[H_contour.pred(H_litem)];

               if(H_pre_sn.label==-2){
                 modules_info[sn.label].x=0;
                 modules_info[sn.label].rx=modules_info[sn.label].width;
               }else{ 
                 modules_info[sn.label].x=modules_info[H_pre_sn.label].rx;
                 modules_info[sn.label].rx=modules_info[sn.label].x+
                                      modules_info[sn.label].width;
               }

               //insert new module into the contour
               H_pre_litem=H_contour.insert(sn, H_litem, LEDA::before);
               H_litems[sn.label]=H_pre_litem;

               //delete those succ nodes whose x coordinate smaller
               //or equal to current node
               H_litem=H_contour.succ(H_pre_litem);
               tmp_sn=H_contour[H_litem]; 
               cur_litem=H_litem; 
               while(tmp_sn.label!=-1&&
                     modules_info[tmp_sn.label].rx<=modules_info[sn.label].rx){
                  H_flags[tmp_sn.label]=0;
                  H_litem=H_contour.succ(H_litem);
                  tmp_sn=H_contour[H_litem];
                  H_contour.del(cur_litem);
                  cur_litem=H_litem;      
               }
               break;            
             }else{
               cout<<"Error of contour_packing in file clo_red.cc"<<endl;
               exit(0);                   
             }                 
           }while(1);                

       }else{
          cout<<"Error of contour_packing in file clo_red.cc"<<endl;
          exit(0);
       }    

       old_sn=sn;  
    }

    V_contour.pop();
    Height=modules_info[V_contour.pop().label].ry;
    H_contour.Pop();
    Width=modules_info[H_contour.Pop().label].rx;

    Area=Width*Height;

    H_litem=packing_seq.push(init_sn);
    packing_litems[init_sn.label]=H_litem;
    Cost=Area; 
}

void Clo_Red_Graph::compute_wire_length(){
  int length=0;

  // int num_of_pins=0;
  // compute absolute position
  
  int mx, my, width, height;
  for(int i=0; i < modules_N; i++){
    mx= modules_info[i].x;
    my= modules_info[i].y;
    width=modules[i].width;
    height=modules[i].height;

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

  float x_ratio=(float)Width/modules[modules_N].width;
  float y_ratio=(float)Height/modules[modules_N].height;
  for(int j=0; j < modules[modules_N].pins.size(); j++){
      Pin &p = modules[modules_N].pins[j];
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
  Cost=Wire;
}

void Clo_Red_Graph::compute_cost(float alpha, float beta, 
                    int A, int W){
  
   if(alpha!=0.0&&beta!=0.0)
     Cost=alpha*(double)Area/A+beta*(double)Wire/W;
   else if(alpha==0.0&&beta!=0.0)
     Cost=Wire;
   else if(alpha!=0.0&&beta==0.0)
     Cost=Area;
}
//---------------------------------------------------------------------------
//   Simulated Annealing Permutation Operations
//---------------------------------------------------------------------------
//***************************************************************
// perturbation
//***************************************************************
void Clo_Red_Graph::perturbate1()
{ 
   int success;
   float rotate_rate=0.7;

   do{
      int c;
      success=1;
      if(rotate_rate> rand_01()){
         c=rand()%3;        
         switch(c){  
           case 0:{   //move edges from h closure graph to v closure graph
              int c2=rand()%2;
              if(c2){
                //cout<<"move edges from h graph to v graph"<<endl;
                success=move_edge(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph"<<endl;
                success=move_edge(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 1:{   // reverse direction of an edge in h closure graph
              int c3=rand()%2;
              if(c3){
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
              int c4=rand()%2;
              if(c4){
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
             int c1=rand()%2;
             rotate(c1);
             break;
           } 
         }
      }
   }while(!success);
}

void Clo_Red_Graph::perturbate2()
{ 
   int success;
   float rotate_rate=0.7;

   do{
      int c=rand()%5;
      success=1;
         switch (c) {
           case 0:{
              int c1=rand()%2;
              rotate(c1);
              break;
              }
           case 1:
              //cout<<"interchange two nodes"<<endl;
              interchange();
              break;
           case 2:{   //move edges from h closure graph to v closure graph
              int c2=rand()%2;
              if(c2){
                //cout<<"move edges from h graph to v graph"<<endl;
                success=move_edge(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph"<<endl;
                success=move_edge(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 3:{   // reverse direction of an edge in h closure graph
              int c3=rand()%2;
              if(c3){
                //cout<<"reverse edge in h graph"<<endl;
                success=reverse_edge(h_clo_G, v_clo_G, 'H');
                break;
              }
              else{   // reverse direction of an edge in v closure graph
                //cout<<"reverse edge in v graph"<<endl;
                success=reverse_edge(v_clo_G, h_clo_G, 'V');
                break;
              }}
           case 4:{   //move edges from h closure graph to v closure graph
              int c4=rand()%2;
              if(c4){
                //cout<<"move edges from h graph to v graph and reverse"<<endl;
                success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph and reverse"<<endl;
                success=move_edge_reverse(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
         }
   }while(!success);
}


void Clo_Red_Graph::perturbate3()
{ 
   int success;
   float rotate_rate=0.3;

   do{
      int c;
      success=1;
      if(rotate_rate> rand_01()){
         c=rand()%3;        
         switch(c){  
           case 0:{   //move edges from h closure graph to v closure graph
              int c2=rand()%2;
              if(c2){
                //cout<<"move edges from h graph to v graph"<<endl;
                success=move_edge(h_clo_G, v_clo_G, 'V');
                break;
              }else{
                //cout<<"move edges from v graph to h graph"<<endl;
                success=move_edge(v_clo_G, h_clo_G, 'H'); 
                break;
              }} 
           case 1:{   // reverse direction of an edge in h closure graph
              int c3=rand()%2;
              if(c3){
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
              int c4=rand()%2;
              if(c4){
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
             int c1=rand()%2;
             rotate(c1);
             break;
           } 
         }
      }
   }while(!success);
}

//***************************************************************
// Pick_Red_Edge is used to pick a reduction edge
//***************************************************************
int Clo_Red_Graph::Pick_Red_Edge(char c, leda_edge& pick_e, 
                                 int& s_label, int& t_label,
                                 GRAPH<int, int>& G)
{
   if(G.number_of_edges()!=0){
     leda_edge e=G.choose_edge();
     int index=G[G.source(e)];
     list_item litem=packing_litems[index];
     Snode init_sn=packing_seq[litem], sn;
     char cur_c;
     leda_list<Snode> modules;
     vector<list_item> module_litems;

     litem=packing_seq.succ(litem);
     while(litem!=nil){
       sn=packing_seq[litem]; 
       cur_c=triangle_matrix.get_edge_type_c(init_sn.pt, sn.pt);
   
       //the edge (init_module, module) appears in the same graph
       if(c==cur_c){
         modules.append(sn);
       }

       litem=packing_seq.succ(litem);
     }
     Snode first_sn, s_sn;
     s_sn=modules.pop();
     first_sn=s_sn;
     forall_items(litem, modules){
       sn=modules[litem];
       cur_c=triangle_matrix.get_edge_type_c(s_sn.pt, sn.pt);
      
       //the edge (s_module, module) appears in the same graph
       if(cur_c==c)
         modules.del(litem);
       else{
         s_sn=sn;
         module_litems.push_back(litem);
       }   
     }

     litem=modules.append(first_sn);
     module_litems.push_back(litem);
     int size=modules.size();
     if(size>=1){
       index=rand()%size;
       sn=modules[module_litems[index]];
       s_label=init_sn.label;
       t_label=sn.label;
       pick_e=triangle_matrix.get_edge(init_sn.pt, sn.pt);
       return 1;
     }else
       return 0;
   }else
     return 0;
}
//***************************************************************
//Tuning graphs while add edges
//***************************************************************
void Clo_Red_Graph::Tune_Closure_Graphs(GRAPH<int, int>& CG_to,
                                        GRAPH<int, int>& CG_from,
                                        leda_node edge_source_node,
                                        leda_node edge_target_node, char d)
{
    int edge_source_label=CG_to[edge_source_node];
    int edge_target_label=CG_to[edge_target_node];
    
    //incremental update edges
    leda_edge se, te, e;
    leda_node sn, tn;
    int s_label, t_label;
    int s_pt, t_pt;

    t_pt=packing_seq[packing_litems[edge_target_label]].pt;
    forall_in_edges(se, edge_source_node){
      sn=CG_to.source(se);
      s_label=CG_to[sn];  
      //cout<<"s_label="<<s_label<<endl;  
      s_pt=packing_seq[packing_litems[s_label]].pt;
      //cout<<"s_pt="<<s_pt<<endl;

      if(triangle_matrix.get_edge_type_c(s_pt, t_pt)!=d){
          e=CG_to.new_edge(sn, edge_target_node);
          triangle_matrix.update_matrix(s_pt, t_pt, 
                                        d, CG_from, e);    
      }
    }
   
    s_pt=packing_seq[packing_litems[edge_source_label]].pt;
    forall_out_edges(te, edge_target_node){
      tn=CG_to.target(te);
      t_label=CG_to[tn];    
      t_pt=packing_seq[packing_litems[t_label]].pt;
      
      if(triangle_matrix.get_edge_type_c(s_pt, t_pt)!=d){
          e=CG_to.new_edge(edge_source_node, tn);
          triangle_matrix.update_matrix(s_pt, t_pt, 
                                        d, CG_from, e);    
      }
    }

    forall_in_edges(se, edge_source_node){
      sn=CG_to.source(se);
      s_label=CG_to[sn];
      s_pt=packing_seq[packing_litems[s_label]].pt;
      forall_out_edges(te, edge_target_node){
        tn=CG_to.target(te);
        t_label=CG_to[tn];    
        t_pt=packing_seq[packing_litems[t_label]].pt;

        if(triangle_matrix.get_edge_type_c(s_pt, t_pt)!=d){
            e=CG_to.new_edge(sn, tn);
            triangle_matrix.update_matrix(s_pt, t_pt, 
                                          d, CG_from, e);    
        }
      }
    }     
}


//***************************************************************
////operator 1::move an edge from one graph to another
//***************************************************************
int Clo_Red_Graph::move_edge(GRAPH<int, int>& CG_from,
                             GRAPH<int, int>& CG_to, char d)
{
   leda_edge move_edge, e;
   leda_node source_node, target_node;
   int source_label, target_label;
   int success=1;
   
   if(d=='H')
     success=Pick_Red_Edge('V', move_edge, source_label, target_label,
                           CG_from);
   else
     success=Pick_Red_Edge('H', move_edge, source_label, target_label,
                           CG_from);

   if(success){
     if(d=='H'){
       source_node=h_nodes[source_label];
       target_node=h_nodes[target_label];
     }else{
       source_node=v_nodes[source_label];
       target_node=v_nodes[target_label];
     }

     //add the new edge to the graph
     e=CG_to.new_edge(source_node, target_node);
     int s_pt=packing_seq[packing_litems[source_label]].pt;
     int t_pt=packing_seq[packing_litems[target_label]].pt;
     triangle_matrix.update_matrix(s_pt, t_pt, d, CG_from, e);    
     
     Tune_Closure_Graphs(CG_to, CG_from, source_node, target_node, d);  
     return 1;
   }
   else
     return 0;
}

//***************************************************************
////operator 2::reverse edge direction in one graph
//***************************************************************
//operator 2::reverse edge direction in one graph
int Clo_Red_Graph::reverse_edge(GRAPH<int, int>& CG_invert,
                    GRAPH<int, int>& CG, char d)
{
   leda_edge invert_edge, e;
   leda_node source_node, target_node;
   int source_label, target_label;
   int success=1;
   char d_new;

   success=Pick_Red_Edge(d, invert_edge, source_label, target_label,
                         CG_invert);

   if(success){
     //delete the edge from G and add a new edge to G
     list_item litem_s=packing_litems[source_label];
     list_item litem_t=packing_litems[target_label];
     int s_pt=packing_seq[litem_s].pt; 
     int t_pt=packing_seq[litem_t].pt;

     if(d=='H'){
        source_node=h_nodes[source_label];
        target_node=h_nodes[target_label];
     }else{
        source_node=v_nodes[source_label];
        target_node=v_nodes[target_label];
     }

     e=CG_invert.new_edge(target_node, source_node);
     triangle_matrix.update_matrix(t_pt, s_pt, d, CG_invert, e);

     //update packing sequence
     list_item litem_last, litem_tmp, litem;
     litem_last=packing_seq.insert(packing_seq[litem_s], 
                              litem_t, LEDA::after);
     packing_litems[source_label]=litem_last;

     litem=packing_seq.succ(litem_s);
     packing_seq.del(litem_s);

     while(litem!=litem_t){
        d_new=triangle_matrix.get_edge_type_c(packing_seq[litem].pt, t_pt);
        litem_tmp=litem;
        litem=packing_seq.succ(litem);        

        if(d_new!=d){
           litem_last=packing_seq.insert(packing_seq[litem_tmp], litem_last,
                                         LEDA::after);
           packing_litems[packing_seq[litem_tmp].label]=litem_last;
           packing_seq.del(litem_tmp);
        } 
     }
     
     //tuning graphs    
     Tune_Closure_Graphs(CG_invert, CG, target_node, source_node, d); 
     return 1;
   }
   else
     return 0;
}
//***************************************************************
//operator 3::rotate a module 
//***************************************************************
void Clo_Red_Graph::rotate(int clockwised){
   int node_label=rand()%modules_N; 
  
   //clockwised==0, rotate positive 90 degree 
   //clockwised==1, rotate negative 90 degree
   int back_up=modules_info[node_label].width;
   modules_info[node_label].width=modules_info[node_label].height;
   modules_info[node_label].height=back_up;

  if(clockwised==0){
    modules_info[node_label].degree++;

    if(modules_info[node_label].degree==4)
      modules_info[node_label].degree=0;
  }else{
    modules_info[node_label].degree--; 

    if(modules_info[node_label].degree==-1)
      modules_info[node_label].degree=3;
  }
}
//***************************************************************
//operator 4::interchang two modules 
//***************************************************************
void Clo_Red_Graph::interchange()
{
   int node_label1, node_label2;
   int pt;
   Snode sn1, sn2, sn;  
  
   node_label1=rand()%modules_N;
   do{
      node_label2=rand()%modules_N;
   }while(node_label1==node_label2);

   list_item litem;

   //update packing sequence
   list_item litem1, litem2;

   litem1=packing_litems[node_label1];
   litem2=packing_litems[node_label2];
   packing_seq[litem1].label=node_label2;
   packing_seq[litem2].label=node_label1;
   packing_litems[node_label1]=litem2;
   packing_litems[node_label2]=litem1;

   //exchange the nodes in h_nodes
   h_clo_G.assign(h_nodes[node_label1], node_label2);
   h_clo_G.assign(h_nodes[node_label2], node_label1);
   v_clo_G.assign(v_nodes[node_label1], node_label2);
   v_clo_G.assign(v_nodes[node_label2], node_label1);

   leda_node n;
   n=h_nodes[node_label1];
   h_nodes[node_label1]=h_nodes[node_label2];
   h_nodes[node_label2]=n;
  
   n=v_nodes[node_label1];
   v_nodes[node_label1]=v_nodes[node_label2];
   v_nodes[node_label2]=n;
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
   char d_new;

   //list_information();
   if(d=='H')
     success=Pick_Red_Edge('V', move_edge, source_label, target_label,
                           CG_from);
   else
     success=Pick_Red_Edge('H', move_edge, source_label, target_label,
                           CG_from);

   if(success){
     list_item litem_s=packing_litems[source_label];
     list_item litem_t=packing_litems[target_label];
     int s_pt=packing_seq[litem_s].pt; 
     int t_pt=packing_seq[litem_t].pt;

     if(d=='H'){
        source_node=h_nodes[source_label];
        target_node=h_nodes[target_label];
     }else{
        source_node=v_nodes[source_label];
        target_node=v_nodes[target_label];
     }

     leda_edge e=CG_to.new_edge(target_node, source_node);
     triangle_matrix.update_matrix(t_pt, s_pt, d, CG_from, e);

     //update packing sequence
     list_item litem_last, litem_tmp, litem;
     litem_last=packing_seq.insert(packing_seq[litem_s], 
                              litem_t, LEDA::after);
     packing_litems[source_label]=litem_last;

     litem=packing_seq.succ(litem_s);
     packing_seq.del(litem_s);

     while(litem!=litem_t){
        d_new=triangle_matrix.get_edge_type_c(s_pt, packing_seq[litem].pt);
        litem_tmp=litem;
        litem=packing_seq.succ(litem);        

        if(d_new==d){
           litem_last=packing_seq.insert(packing_seq[litem_tmp], litem_last,
                                         LEDA::after);
           packing_litems[packing_seq[litem_tmp].label]=litem_last;
           packing_seq.del(litem_tmp);
        } 
     }
     Tune_Closure_Graphs(CG_to, CG_from, target_node, source_node, d);  
     return 1;
   }
   else
     return 0;
}

//---------------------------------------------------------------
//   Simulated Annealing Temporal Solution
//---------------------------------------------------------------

float Clo_Red_Graph::get_cost(){
  return Cost;
}

void copy_graph(GRAPH<int, int>& from_G, GRAPH<int, int>& G,
                Hv_Matrix& triangle_matrix, char d, vector<leda_node>& v)
{
   G.clear();
   leda_edge e1, e2;
   leda_node n1, n2;
   int label;

   forall_nodes(n1, from_G)
   {
      n2=G.new_node();
      label=from_G.inf(n1);
      G.assign(n2, label);
      v[label]=n2;
   }

   forall_edges(e1, from_G)
   {
      int s_label=from_G.inf(from_G.source(e1));
      int t_label=from_G.inf(from_G.target(e1));
      e2=G.new_edge(v[s_label], v[t_label]);
      triangle_matrix.set_edge(s_label, t_label, e2);
      triangle_matrix.set_edge_type(s_label, t_label, d);
   }   
}

void Clo_Red_Graph::reconstruct(leda_list<Snode>& packing_seq_from, 
              leda_list<Snode>& packing_seq,
              vector<list_item>& packing_litems,
              GRAPH<int, int>& G_h,  GRAPH<int, int>& G_v,
              vector<leda_node>& v_h, vector<leda_node>& v_v,
              Hv_Matrix& triangle_matrix_from, Hv_Matrix& triangle_matrix){ 
   list_item litem;
   Snode sn;
   packing_seq.clear();
   G_h.clear();
   G_v.clear();
   leda_node n;

   forall(sn, packing_seq_from){
        //sn.pt=sn.label;
        litem=packing_seq.append(sn);
        packing_litems[sn.label]=litem;

        n=G_h.new_node();
        G_h.assign(n, sn.label);
        v_h[sn.label]=n;

        n=G_v.new_node();
        G_v.assign(n, sn.label);
        v_v[sn.label]=n;
   }

   int s_label, t_label;
   int s_pt, t_pt;
   int i, j=1;
   list_item litem_s, litem_t;
   litem_s=packing_seq.first();
   char d;
   leda_edge e;

   do{
        s_label=packing_seq[litem_s].label;
        s_pt=packing_seq[litem_s].pt;
        litem_t=packing_seq.succ(litem_s);
        do{
            t_label=packing_seq[litem_t].label;
            t_pt=packing_seq[litem_t].pt;
            d=triangle_matrix_from.get_edge_type_c(s_pt, t_pt);
 
            if(d=='H'){
               e=G_h.new_edge(v_h[s_label], v_h[t_label]);
               triangle_matrix.set_edge(s_pt, t_pt, e);
               triangle_matrix.set_edge_type(s_pt, t_pt, d);
            }else{
               e=G_v.new_edge(v_v[s_label], v_v[t_label]);
               triangle_matrix.set_edge(s_pt, t_pt, e);
               triangle_matrix.set_edge_type(s_pt, t_pt, d);
            }
            litem_t=packing_seq.succ(litem_t);
        }while(litem_t!=nil);
        litem_s=packing_seq.succ(litem_s); 
   }while(litem_s!=packing_seq.last());   
}


void copy_seq(leda_list<Snode>& packing_seq_from, 
              leda_list<Snode>& packing_seq,
              vector<list_item>& packing_litems){
   list_item litem;
   Snode sn;
   packing_seq.clear();

   forall(sn, packing_seq_from){
        sn.pt=sn.label;
        litem=packing_seq.append(sn);
        packing_litems[sn.label]=litem;
   }
}


void Clo_Red_Graph::get_solution(Solution &sol){
  triangle_matrix.copy_edge_types(sol.triangle_matrix);
  sol.packing_seq=packing_seq;
  sol.Area= Area;
  sol.Width=Width;
  sol.Height=Height;
  sol.Wire= Wire;
  sol.Cost = Cost;
  sol.modules_info=modules_info;
}

void Clo_Red_Graph::recover(Solution &sol){
  reconstruct(sol.packing_seq, packing_seq, packing_litems,
              h_clo_G, v_clo_G, h_nodes, v_nodes,
              sol.triangle_matrix, triangle_matrix);
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
   int s_pt, t_pt;
 
   forall_edges(e, h_clo_G)
   {
       source_label=h_clo_G[h_clo_G.source(e)];
       target_label=h_clo_G[h_clo_G.target(e)];
       s_pt=packing_seq[packing_litems[source_label]].pt;
       t_pt=packing_seq[packing_litems[target_label]].pt;
       //cout<<"source_label="<<source_label
       //    <<", target_label="<<target_label<<endl;       
       //cout<<"s_pt="<<s_pt<<", t_pt="<<t_pt<<endl;
       if(!triangle_matrix.equal_type(s_pt, t_pt, 'H'))
       {
           cout<<"type error of h_clo_G in clo_red.cc!!!"<<endl;
           cout<<"<"<<source_label<<", "<<target_label<<">"<<endl;
           list_information();
           exit(0);
       }
   }


   forall_edges(e, v_clo_G)
   {
       source_label=v_clo_G[v_clo_G.source(e)];
       target_label=v_clo_G[v_clo_G.target(e)];
       s_pt=packing_seq[packing_litems[source_label]].pt;
       t_pt=packing_seq[packing_litems[target_label]].pt;       
       
       if(!triangle_matrix.equal_type(s_pt, t_pt, 'V'))
       {
               cout<<"type error of v_clo_G in clo_red.cc!!!"<<endl;
               cout<<"<"<<source_label+1<<", "<<target_label+1<<">"<<endl;
               list_information();
               exit(0);
       }
   }
   list_item litem_s, litem_t;
   litem_s=packing_seq.first();
  
   do{
      litem_t=packing_seq.succ(litem_s);
      do{
         s_pt=packing_seq[litem_s].pt;
         t_pt=packing_seq[litem_t].pt;
  
         if(s_pt<t_pt){ 
            if(triangle_matrix.get_edge_type(s_pt, t_pt)==1||
               triangle_matrix.get_edge_type(s_pt, t_pt)==3){
               cout<<"s="<<packing_seq[litem_s].label<<", t="
                         <<packing_seq[litem_t].label<<endl;
               cout<<"s_pt="<<packing_seq[litem_s].pt<<", t_pt="
                            <<packing_seq[litem_t].pt<<endl;
               cout<<"packing seq error"<<endl;
               list_information();
               exit(0);
            }
         }else{
           if(triangle_matrix.get_edge_type(s_pt, t_pt)==0||
               triangle_matrix.get_edge_type(s_pt, t_pt)==2){
               cout<<"s="<<packing_seq[litem_s].label<<", t="
                         <<packing_seq[litem_t].label<<endl;
               cout<<"s_pt="<<packing_seq[litem_s].pt<<", t_pt="
                            <<packing_seq[litem_t].pt<<endl;
               cout<<"packing seq error"<<endl;
               list_information();
               exit(0);
            }
         } 
         litem_t=packing_seq.succ(litem_t);
      }while(litem_t!=nil);
      litem_s=packing_seq.succ(litem_s);
   }while(litem_s!=packing_seq.last());
}

void Clo_Red_Graph::list_information()
{
      leda_edge e;
      Snode sn;

      cout<<"*******************************************"<<endl;
      cout<<"transitive closure horizontal graph"<<endl;
      forall_edges(e, h_clo_G){
         cout<<"<"<<h_clo_G[h_clo_G.source(e)]<<", "
             <<h_clo_G[h_clo_G.target(e)]<<">"<<endl;
      }

      cout<<"*******************************************"<<endl;
      cout<<"transitive closure vertical graph"<<endl;
      forall_edges(e, v_clo_G){
         cout<<"<"<<v_clo_G[v_clo_G.source(e)]<<", "
             <<v_clo_G[v_clo_G.target(e)]<<">"<<endl;
      }

      cout<<"*******************************************"<<endl;
      cout<<triangle_matrix;

      cout<<"*******************************************"<<endl;
      forall(sn, packing_seq){
         cout<<"label="<<sn.label
             <<", pt="<<sn.pt<<endl;
      }

      cout<<"*******************************************"<<endl;
      cout<<"Best Area="<<get_cost()<<endl;
}

float rand_01(){
  return float(rand()%10000)/10000;
}
