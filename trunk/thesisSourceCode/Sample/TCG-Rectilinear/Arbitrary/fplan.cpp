#include "fplan.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <climits>
//---------------------------------------------------------------------------
void FloorPlan::rotate_module(int index, float& new_width, float& new_height,
                              int f)
{
   float back_up=modules_info[index-1].width;
   modules_info[index-1].width=modules_info[index-1].height;
   modules_info[index-1].height=back_up; 

   new_width=modules_info[index-1].width;
   new_height=modules_info[index-1].height;
 
  if(f==0){
    modules_info[index-1].degree++; 

    if(modules_info[index-1].degree==4)
      modules_info[index-1].degree=0;
  }else{
    modules_info[index-1].degree--; 

    if(modules_info[index-1].degree==-1)
      modules_info[index-1].degree=3;     
  } 
}
//---------------------------------------------------------------------------
char line[100],t1[40],t2[40];
ifstream fs;

char* tail(char *str){
    str[strlen(str)-1]=0;
    return str;
}

void FloorPlan::read(char *file){

  fs.open(file);
  if(fs==NULL)
    error("unable to open file: %s",file);

  bool final=false;
  Module dummy_mod;
  for(int i=0; !fs.eof(); i++){
    // modules
    Pmodules.push_back(dummy_mod);	// new module
    Module &mod = Pmodules.back();
    mod.id = i;
    mod.pins.clear();

    fs >> t1 >> t2;
    tail(t2);			// remove ";"
    strcpy(mod.name,t2);

    fs >> t1 >> t2;
    if(!strcmp(t2,"PARENT;"))
	final= true;
    
    // dimension
    read_dimension(mod);    
    read_IO_list(mod,final);

    // network
    if(final){
      read_network();
      break;
    }
  }
  create_network();

  root_module = Pmodules.back();
  modules_N = Pmodules.size()-1;  // exclude the parent module  
  fs.close();
}

void FloorPlan::read_dimension(Module &mod){
    fs >> t1;
    int min_x=INT_MAX,min_y=INT_MAX,max_x=INT_MIN,max_y=INT_MIN;
    int tx,ty;
    for(int i=0; i < 4;i++){
      fs >> tx >> ty; 
      min_x=min(min_x,tx); max_x=max(max_x,tx);
      min_y=min(min_y,ty); max_y=max(max_y,ty);
    }

    mod.x      = min_x;
    mod.y      = min_y;
    mod.width  = max_x - min_x;
    mod.height = max_y - min_y;
    mod.area   = mod.width * mod.height;
    fs >> t1 >> t2;
}

void FloorPlan::read_IO_list(Module &mod,bool parent=false){
    // IO list
    while(!fs.eof()){
      Pin p;
      fs.getline(line,100);
      if(strlen(line)==0) continue;
      sscanf(line,"%s %*s %f %f",t1,&p.x,&p.y);

      if(!strcmp(t1,"ENDIOLIST;"))
	break;

      if(parent){ // IO pad is network
       // make unique net id
        net_table.insert(make_pair(string(t1),net_table.size()));
        p.net = net_table[t1];
      }

      p.mod = mod.id;
      p.x -= mod.x;  p.y -= mod.y;	// shift to origin
      mod.pins.push_back(p);
    }   
    fs.getline(line,100);  
}

void FloorPlan::read_network(){
    while(!fs.eof()){
      bool end=false;
      int n=0;
      fs >> t1 >> t2;
      if(!strcmp(t1,"ENDNETWORK;"))
        break;
      // determine which module interconnection by name
      int m_id;
      for(m_id=0; m_id < Pmodules.size(); m_id++)
        if(!strcmp(Pmodules[m_id].name,t2))
   	  break;
      if(m_id== Pmodules.size())
 	error("can't find suitable module name!");
        
      while(!fs.eof()){
        fs >> t1;
        if(t1[strlen(t1)-1]==';'){
 	  tail(t1);
          end=true;
        }

        // make unique net id
        net_table.insert(make_pair(string(t1),net_table.size()));
        Pmodules[m_id].pins[n++].net = net_table[t1];
        if(end) break;
      }
    }
}

void FloorPlan::create_network(){
  network.resize(net_table.size());

  for(int i=0; i < Pmodules.size(); i++){
    for(int j=0; j < Pmodules[i].pins.size(); j++){
      Pin &p = Pmodules[i].pins[j];
      network[p.net].push_back(&p);     
    }
  }  
   
}  

string query_map(map<string,int> M,int value){
  for(map<string,int>::iterator p=M.begin(); p != M.end(); p++){
    if(p->second == value)
      return p->first;
  }
  return "";
}

void FloorPlan::show_modules()
{
  for(int i=0; i < modules.size();i++){
    cout <<i+1<< "*Module: " << modules[i].id+1 << endl;
    cout << "  Width = " << modules[i].width;
    cout << "  Height= " << modules[i].height << endl;
    cout << "  Area  = " << modules[i].area << endl;
  }
  cout<<"Area="<<Area<<endl;
  cout<<"***********************************************************************"<<endl;
}

void FloorPlan::show_modules_pins()
{
  for(int i=0; i < modules.size();i++){
    cout << "**Module: " << Pmodules[i].name << endl;
    cout << "  Width = " << Pmodules[i].width;
    cout << "  Height= " << Pmodules[i].height << endl;
    cout << "  Area  = " << Pmodules[i].area << endl;
    cout << "  <X, Y> = <" <<modules_info[i].x << ", " << modules_info[i].y<<">"<<endl;
    cout << "  Degree = "<< modules_info[i].degree*90<<endl; 
    cout << "  "<<Pmodules[i].pins.size() << " Pins:\n";
//    for(int j=0; j < modules[i].pins.size(); j++){
//      cout << query_map(net_table,modules[i].pins[j].net) << " ";
//      cout << modules[i].pins[j].x << " " << modules[i].pins[j].y << endl;
//      cout << "after rotate=";
//      cout << modules[i].pins[j].ax<< " " << modules[i].pins[j].ay<<endl;
//    }
  }
  cout<<"Area="<<Area<<endl;
  cout<<"***********************************************************************"<<endl;
}


void error(char *msg,char *msg2=""){
  printf(msg,msg2);
  cout << endl;
  throw 1;
}
//-----------------------------------------------------------------------------
// HsinLung
//-----------------------------------------------------------------------------
void FloorPlan::ReadRectilinear(char *file){

  fs.open(file);
  if(fs==NULL)
    error("unable to open file: %s",file);

  bool final=false;
  Module dummy_mod;
  int count=0;
  char type;  	
  Num_of_Hard=0;
  Num_of_L=0;
  fs >> type;	
  for(int i=0; !fs.eof(); i++){
    // modules	  
    modules.push_back(dummy_mod);	// new module
    Module &mod = modules.back();
    //mod.id = i;
    mod.pins.clear();  
    switch(type){
      case 'H' :  mod.type = MT_Hard;
      		  Num_of_Hard++;
    		  Read_RDimension(mod);
		  fs >> type;
		  break; 
      case 'L' :  mod.type = MT_ReclinearL;
           	  Num_of_L++;
		  mod.submodules.resize(1);
		  if (count<1){
		    Read_RDimension(mod);
		    mod.submodules[0].id = i+1;
		    count++;
		  }
                  else{
		    Read_RDimension(mod);
		    mod.submodules[0].id = i-1;
		    Compute_relation(type, i);
		    fs >> type;	 
		    count=0;
		  }
	          //cout<<mod.submodules[0]<<" ";
		  break; 
      case 'T' :  mod.type = MT_ReclinearT;
		  mod.submodules.resize(2);
		  if (count==0){
		    Read_RDimension(mod);
		    mod.submodules[0].id = i+1;	
		    mod.submodules[1].id = i+2;
		    count++;
		  }
		  else if (count==1){
		    Read_RDimension(mod);
		    mod.submodules[0].id = i-1;	
		    mod.submodules[1].id = i+1;
		    count++;
		  }
                  else{
		    Read_RDimension(mod);
		    mod.submodules[0].id = i-2;	
		    mod.submodules[1].id = i-1;
		    Compute_relation(type, i);
		    fs >> type;	 
		    count=0;
		  }
		  break;
      default  :  fs >> type;break;	   
    } 	

  }

  modules_N = modules.size();  // exclude the parent module
  modules_info.resize(modules_N);
  //modules.resize(modules_N);

  TotalArea = 0;
  for(int i=0; i < modules_N; i++){
    TotalArea += modules[i].area;
    modules_info[i].width=modules[i].width;
    modules_info[i].height=modules[i].height;
    if (modules[i].width==Pmodules[modules[i].id].width && modules[i].height==Pmodules[modules[i].id].height)
    {
    	modules_info[i].degree=0;
    }
    else if (modules[i].width==Pmodules[modules[i].id].height && modules[i].height==Pmodules[modules[i].id].width)
    {
    	modules_info[i].degree=1;
    }
    else
    {
    	cout<<"Dimension error in reading files"<<endl;
    	exit(0);
    }
    modules_info[i].relation=0;
  }
  //cout<<"TotalArea="<<TotalArea<<endl;
  //show_modules();
  //show_modules_pins();
  
}

void FloorPlan::Read_RDimension(Module &mod){
    fs >> mod.id ;    
    mod.id--;
    fs >> mod.width >> mod.height;
    mod.area   = mod.width * mod.height;	    
}

void FloorPlan::Compute_relation(char type, int i)
{  float p1,p2;
   int j=i-1;
   int k=i-2;
   switch(type)
   {
      case 'L' :  fs >> p1;
		  modules[i].submodules[0].relation1=0-p1;
		  modules[i].submodules[0].relation2=modules[i].height-modules[j].height+p1;
		  modules[j].submodules[0].relation1=p1;
		  modules[j].submodules[0].relation2=modules[j].height-modules[i].height-p1;
                  break;
      case 'T' :  fs >> p1 >> p2;
		  modules[k].submodules[0].relation1=p1;
		  modules[k].submodules[0].relation2=modules[k].height-modules[j].height-p1;
		  modules[k].submodules[1].relation1=p2;
		  modules[k].submodules[1].relation2=modules[k].height-modules[i].height-p2;
		  modules[j].submodules[0].relation1=0-p1;
		  modules[j].submodules[0].relation2=modules[j].height-modules[k].height+p1;
		  modules[j].submodules[1].relation1=p2-p1;
		  modules[j].submodules[1].relation2=modules[j].height-modules[i].height+p1-p2;
		  modules[i].submodules[0].relation1=0-p2;
 		  modules[i].submodules[0].relation2=modules[i].height-modules[k].height+p2;
		  modules[i].submodules[1].relation1=p1-p2;
		  modules[i].submodules[1].relation2=modules[i].height-modules[j].height-p1+p2;
                  break;
   }
}
//------------------------------------------------------------------------------