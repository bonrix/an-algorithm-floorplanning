

//#include <vector>
//#include <string.h>
//#include <map>
//#include <stdio.h>

//#include <iostream>
//#include <fstream>

//---------------------------------------------------------------------------

#include <vector>
//#include <string.h>
#include <iostream> // standard streams
#include <fstream> // file streams
#include <sstream> // string streams
//#include <time.h>
using namespace std;
#include <map>




#include "floorplan.h"

char line[100],t1[40],t2[40];

ifstream fs;

FloorPlan::FloorPlan(){
  norm_area= 1;
  norm_wire= 1;
  
}


void FloorPlan::clear(){
  Area = 0; 
}

double FloorPlan::getCost(){
  
     return (Area/norm_area);
}

float FloorPlan::getDeadSpace(){
  return 100*(Area-TotalArea)/float(Area);
}

void FloorPlan::normalize_cost(int t){
  norm_area=norm_wire=0;

  for(int i=0; i < t; i++){
    
    norm_area += Area;
   
  }
  
  norm_area /= t;
  norm_wire /= t;
  printf("normalize area=%.0f, wire=%.0f\n", norm_area, norm_wire);
}

//---------------------------------------------------------------------------
//   Read
//---------------------------------------------------------------------------

char* tail(char *str){
    str[strlen(str)-1]=0;
    return str;
}

/*void FloorPlan::read(char *file){
  filename = file; 
  fs.open(file);
  if(fs==NULL)
    error("unable to open file: %s",file);

  bool final=false;
  Module dummy_mod;
  for(int i=0; !fs.eof(); i++){
    // modules
    modules.push_back(dummy_mod);	// new module
    Module &mod = modules.back();
    mod.id = i;
   

    fs >> t1 >> t2;
    tail(t2);			// remove ";"
    strcpy(mod.name,t2);

    fs >> t1 >> t2;
    if(!strcmp(t2,"PARENT;"))
	final= true;
    
    // dimension
 // dimension
    read_dimension(mod);  
	read_IO_list(mod,final);

    // network
    if(final){
      read_network();
      break;
    }
  
  }

  root_module = modules.back();
  modules.pop_back();		// exclude the parent module
  modules_N = modules.size();  
  modules_info.resize(modules_N);
  modules.resize(modules_N);

   TotalArea = 0;
  for( i=0; i < modules_N; i++)
    TotalArea += modules[i].area;

}*/


void FloorPlan::read(char *file){
  filename = file; 
  fs.open(file);
  if(fs==NULL)
    error("unable to open file: %s",file);

  bool final=false;
  Module dummy_mod;
  for(int i=0; !fs.eof(); i++){
    // modules
    modules.push_back(dummy_mod);	// new module
    Module &mod = modules.back();
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

  root_module = modules.back();
  modules.pop_back();		// exclude the parent module
  modules_N = modules.size();  
  modules_info.resize(modules_N);
  modules.resize(modules_N);

 // create_network();

  TotalArea = 0;
  for(i=0; i < modules_N; i++)
    TotalArea += modules[i].area;
	minTotalArea += modules[i].area;

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
      sscanf(line,"%s %*s %d %d",t1,&p.x,&p.y);

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
      for(m_id=0; m_id < modules.size(); m_id++)
        if(!strcmp(modules[m_id].name,t2))
   	  break;
      if(m_id== modules.size())
 	error("can't find suitable module name!");
        
      while(!fs.eof()){
        fs >> t1;
        if(t1[strlen(t1)-1]==';'){
 	  tail(t1);
          end=true;
        }

        // make unique net id
        net_table.insert(make_pair(string(t1),net_table.size()));
        modules[m_id].pins[n++].net = net_table[t1];
        if(end) break;
      }
    }
}


//---------------------------------------------------------------------------
//   Modules Information
//---------------------------------------------------------------------------

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
	cout << "Module: " << modules[i].name << endl;
    cout << "  Width = " << modules[i].width;
    cout << "  Height= " << modules[i].height << endl;
    cout << "  Area  = " << modules[i].area << endl;
//    cout << modules[i].pins.size() << " Pins:\n";
//    for(int j=0; j < modules[i].pins.size(); j++){
//      cout << query_map(net_table,modules[i].pins[j].net) << " ";
//      cout << modules[i].pins[j].x << " " << modules[i].pins[j].y << endl;
//    }
  }
}

void FloorPlan::  setInOrder(){
	int  j, pass;
	Module hold;
	bool switched = true;
	int n = modules.size();
	for (pass = 0; pass < n-1 && switched == true; pass++) {
		switched = false;
		for (j=0; j<n-pass-1; j++) {
			if (modules[j].area>modules[j+1].area) {
				switched = true;
				hold = modules[j] ;
				modules[j] = modules[j+1];
				modules[j+1] = hold;
			}
		}
	}	
}

void setInOrder(SuperModules &superModules){
	int  j, pass;
	SuperModule hold;
	bool switched = true;
	int n = superModules.size();
	for (pass = 0; pass < n-1 && switched == true; pass++) {
		switched = false;
		for (j=0; j<n-pass-1; j++) {
			if (superModules[j].area>superModules[j+1].area) {
				switched = true;
				hold = superModules[j] ;
				superModules[j] = superModules[j+1];
				superModules[j+1] = hold;
			}
		}
	}	
}

void FloorPlan::list_information(){

  string info = filename + ".info"   ;
  ofstream of(info.c_str());
  
  of << modules_N << " " << Width << " " << Height << endl;
  for(int i=0; i < modules_N; i++){
    of << modules_info[i].x  << " " << modules_info[i].y   << " ";
    of << modules_info[i].rx << " " << modules_info[i].ry << endl;
  }
  of << endl;

  
  cout << "Num of Module  = " << modules_N << endl;
  cout << "Height         = " << Height*1e-3 << endl;
  cout << "Width          = " << Width*1e-3 << endl;
  cout << "Area           = " << Area*1e-6 << endl;
  cout << "Total Area     = " << TotalArea*1e-6 << endl;
  printf( "Dead Space     = %.2f\n", getDeadSpace());
 // printf( "Cost			  = %.6f\n", float(getCost()));
  
}


void SuperModule :: setChild(Modules &modules, int intialPosition, int childCount, bool isItLeaf){
  moduleCount = childCount;
  isLeaf = isItLeaf;
  childModule[0] = &modules[intialPosition];
}

void SuperModule :: setChildComposite(SuperModules &superModules, int intialPosition, int childCount, bool isItLeaf){
  moduleCount = childCount;
  isLeaf = isItLeaf;
  for(int i=0; i < moduleCount;i++){
	childModule[i] = &superModules[intialPosition+i];
  }
}

void SuperModule :: arrangeBlocks() {
	switch (moduleCount) {
	case 4:
		arrange4Blocks();
		break;
	case 3:
		arrange3Blocks();
		break;
	case 2:
		arrange2Blocks();
		break;
	case 1:
		arrange1Blocks();
		break;
	default:
		cout << "error modules count other than 4,3,2,1";
	}
}


void SuperModule :: arrange4Blocks() {
	int expHeight[4];
	int expWidth[4];
	width = INT_MAX;
	height = INT_MAX;
	area = INT_MAX;
	int tempWidth;
	int tempHeight;
	int tempArea;
	for(int i=0; i < 24;i++) {
		for(int j=0; j < 16;j++) {
			for(int l=0; l < 4; l++){
				switch (modulePattern44[i][l]) {
				case 'a':
					if (moduleRoatation44[j][l]=='0') {
						expHeight[l] = childModule[0]->height;
						expWidth[l] = childModule[0]->width;
					} else {
						expHeight[l] = childModule[0]->width;
						expWidth[l] = childModule[0]->height;
					}
					break;
				case 'b':
					if (moduleRoatation44[j][l]=='0') {
						expHeight[l] = childModule[1]->height;
						expWidth[l] = childModule[1]->width;
					} else {
						expHeight[l] = childModule[1]->width;
						expWidth[l] = childModule[1]->height;
					}
					break;
				case 'c':
					if (moduleRoatation44[j][l]=='0') {
						expHeight[l] = childModule[2]->height;
						expWidth[l] = childModule[2]->width;
					} else {
						expHeight[l] = childModule[2]->width;
						expWidth[l] = childModule[2]->height;
					}
					break;
				case 'd':
					if (moduleRoatation44[j][l]=='0') {
						expHeight[l] = childModule[3]->height;
						expWidth[l] = childModule[3]->width;
					} else {
						expHeight[l] = childModule[3]->width;
						expWidth[l] = childModule[3]->height;
					}
					break;
				}
			}
			for(int k=0; k < 6;k++) {
				switch(k) {
				case placement1:
					tempHeight = max(expHeight[0],expHeight[1],expHeight[2],expHeight[3]);
					tempWidth = expWidth[0] + expWidth[1] + expWidth[2] + expWidth[3];
					break;
				case placement2:
					tempHeight = max(expHeight[0],expHeight[1],expHeight[2] + expHeight[3]);
					tempWidth = expWidth[0] + expWidth[1] +  max(expWidth[2] , expWidth[3]);
					break;
				case placement3:
					tempHeight = max(expHeight[0], expHeight[1] + expHeight[2] + expHeight[3]);
					tempWidth = expWidth[0] + max(expWidth[1], expWidth[2], expWidth[3]);
					break;
				case placement4:
					tempHeight = max(expHeight[0] + expHeight[1] , expHeight[2] + expHeight[3]);
					tempWidth = max(expWidth[0] , expWidth[1]) + max (expWidth[2], expWidth[3]);
					break;
				case placement5:
					tempHeight = max(expHeight[0], expHeight[1] + max(expHeight[2], expHeight[3]));
					tempWidth = expWidth[0] + max (expWidth[1], expWidth[2] + expWidth[3]);
					break;
				case placement6:
					//tempHeight = max(expHeight[0] + expHeight[3] , expHeight[1] + expHeight[2]);
					//tempWidth = max (expWidth[0] + expWidth[1], expWidth[2] + expWidth[3]);
					tempHeight = max(expHeight[0] + expHeight[1] , max(expHeight[0],expHeight[2]) + expHeight[3]);
					tempWidth = max (max(expWidth[0],expWidth[1]) + expWidth[2], expWidth[1] + expWidth[3]);
					break;
				}
				tempArea = 	tempHeight * tempWidth;
				if (tempArea < area ) {
					height = tempHeight;
					width = tempWidth;
					area = tempArea;
					strcpy(selectedPattern, modulePattern44[i]);
					strcpy(selectedRotation,moduleRoatation44[j]);
					selectedPlacement = (placementType) k;
				} else if( tempArea == area ) {
					if ( abs(tempHeight - tempWidth) < abs(height - width) ) {
						height = tempHeight;
						width = tempWidth;
						area = tempArea;
						strcpy(selectedPattern, modulePattern44[i]);
						strcpy(selectedRotation,moduleRoatation44[j]);
						selectedPlacement = (placementType) k;
					}
				}
			}
		}
	}
}

void SuperModule :: arrange3Blocks() {
	int expHeight[3];
	int expWidth[3];
	width = INT_MAX;
	height = INT_MAX;
	area = INT_MAX;
	int tempWidth;
	int tempHeight;
	int tempArea;
	for(int i=0; i < 6;i++) {
		for(int j=0; j < 8;j++) {
			for(int l=0; l < 3; l++){
				switch (modulePattern43[i][l]) {
				case 'a':
					if (moduleRoatation43[j][l]=='0') {
						expHeight[l] = childModule[0]->height;
						expWidth[l] = childModule[0]->width;
					} else {
						expHeight[l] = childModule[0]->width;
						expWidth[l] = childModule[0]->height;
					}
					break;
				case 'b':
					if (moduleRoatation43[j][l]=='0') {
						expHeight[l] = childModule[1]->height;
						expWidth[l] = childModule[1]->width;
					} else {
						expHeight[l] = childModule[1]->width;
						expWidth[l] = childModule[1]->height;
					}
					break;
				case 'c':
					if (moduleRoatation43[j][l]=='0') {
						expHeight[l] = childModule[2]->height;
						expWidth[l] = childModule[2]->width;
					} else {
						expHeight[l] = childModule[2]->width;
						expWidth[l] = childModule[2]->height;
					}
					break;
				}
			}
			for(int k=0; k < 2;k++) {
				switch(k) {
				case placement1:
					tempHeight = max(expHeight[0],expHeight[1],expHeight[2]);
					tempWidth = expWidth[0] + expWidth[1] + expWidth[2];
					break;
				case placement2:
					tempHeight = max(expHeight[0], expHeight[1]+expHeight[2]);
					tempWidth = expWidth[0] + max(expWidth[1], expWidth[2]);
					break;
				}
				tempArea = 	tempHeight * tempWidth;
				if (tempArea < area ) {
					height = tempHeight;
					width = tempWidth;
					area = tempArea;
					strcpy(selectedPattern, modulePattern43[i]);
					strcpy(selectedRotation,moduleRoatation43[j]);
					selectedPlacement = (placementType) k;
				} else if( tempArea == area ) {
					if ( abs(tempHeight - tempWidth) < abs(height - width) ) {
						height = tempHeight;
						width = tempWidth;
						area = tempArea;
						strcpy(selectedPattern, modulePattern43[i]);
						strcpy(selectedRotation,moduleRoatation43[j]);
						selectedPlacement = (placementType) k;
					}
				}
			}
		}
	}
}
void SuperModule :: arrange2Blocks() {
	int expHeight[2];
	int expWidth[2];
	width = INT_MAX;
	height = INT_MAX;
	area = INT_MAX;
	int tempWidth;
	int tempHeight;
	int tempArea;
	for(int i=0; i < 2;i++) {
		for(int j=0; j < 4;j++) {
			for(int l=0; l < 2; l++){
				switch (modulePattern42[i][l]) {
				case 'a':
					if (moduleRoatation42[j][l]=='0') {
						expHeight[l] = childModule[0]->height;
						expWidth[l] = childModule[0]->width;
					} else {
						expHeight[l] = childModule[0]->width;
						expWidth[l] = childModule[0]->height;
					}
					break;
				case 'b':
					if (moduleRoatation42[j][l]=='0') {
						expHeight[l] = childModule[1]->height;
						expWidth[l] = childModule[1]->width;
					} else {
						expHeight[l] = childModule[1]->width;
						expWidth[l] = childModule[1]->height;
					}
					break;
				}
			}
			for(int k=0; k < 1;k++) {
				switch(k) {
				case placement1:
					tempHeight = max(expHeight[0],expHeight[1]);
					tempWidth = expWidth[0] + expWidth[1];
					break;
				}
				tempArea = 	tempHeight * tempWidth;
				if (tempArea < area ) {
					height = tempHeight;
					width = tempWidth;
					area = tempArea;
					strcpy(selectedPattern, modulePattern42[i]);
					strcpy(selectedRotation,moduleRoatation42[j]);
					selectedPlacement = (placementType) k;
				} else if( tempArea == area ) {
					if ( abs(tempHeight - tempWidth) < abs(height - width) ) {
						height = tempHeight;
						width = tempWidth;
						area = tempArea;
						strcpy(selectedPattern, modulePattern42[i]);
						strcpy(selectedRotation,moduleRoatation42[j]);
						selectedPlacement = (placementType) k;
					}
				}
			}
		}
	}
}

void SuperModule :: arrange1Blocks() {
	height = childModule[0]->height;
	width = childModule[0]->width;
	area = height * width;
}


void SuperModule :: setChildCordinates() {
	switch (moduleCount) {
	case 4:
		set4ChildCordinates();
		break;
	case 3:
		set3ChildCordinates();
		break;
	case 2:
		set2ChildCordinates();
		break;
	case 1:
		set1ChildCordinates();
		break;
	default:
		cout << "error modules count other than 4,3,2,1";
	}
}

void SuperModule::set4ChildCordinates() {
	int expHeight[4];
	int expWidth[4];
	
	for(int l=0; l < 4; l++) {
		if(isRotation) {
			if(selectedRotation[l]=='0') selectedRotation[l]='1';
				else selectedRotation[l]='0';
		}
		switch (selectedPattern[l]) {
		case 'a':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[0]->height;
				expWidth[l] = childModule[0]->width;
				((SuperModule *)childModule[0])->isRotation = false;
			} else {
				expHeight[l] = childModule[0]->width;
				expWidth[l] = childModule[0]->height;
				((SuperModule *)childModule[0])->isRotation = true;
			}
			set4ChildCordinatesSetOne(0,l,expHeight,expWidth);
			break;
		case 'b':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[1]->height;
				expWidth[l] = childModule[1]->width;
				((SuperModule *)childModule[1])->isRotation = false;
			} else {
				expHeight[l] = childModule[1]->width;
				expWidth[l] = childModule[1]->height;
				((SuperModule *)childModule[1])->isRotation = true;
			}
			set4ChildCordinatesSetOne(1,l,expHeight,expWidth);
			break;
		case 'c':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[2]->height;
				expWidth[l] = childModule[2]->width;
				((SuperModule *)childModule[2])->isRotation = false;
			} else {
				expHeight[l] = childModule[2]->width;
				expWidth[l] = childModule[2]->height;
				((SuperModule *)childModule[2])->isRotation = true;
			}
			set4ChildCordinatesSetOne(2,l,expHeight,expWidth);
			break;
		case 'd':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[3]->height;
				expWidth[l] = childModule[3]->width;
				((SuperModule *)childModule[3])->isRotation = false;
			} else {
				expHeight[l] = childModule[3]->width;
				expWidth[l] = childModule[3]->height;
				((SuperModule *)childModule[3])->isRotation = true;
			}
			set4ChildCordinatesSetOne(3,l,expHeight,expWidth);
			break;
		}
	}
}

/*
void SuperModule::set4ChildCordinates() {
	int expHeight[4];
	int expWidth[4];
	
	for(int l=0; l < 4; l++) {
		if(isRotation) {
			if(selectedRotation[l]=='0') selectedRotation[l]='1';
				else selectedRotation[l]='0';
		}
		switch (selectedPattern[l]) {
		case 'a':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[0]->height;
				expWidth[l] = childModule[0]->width;
				if (!isRotation) ((SuperModule *)childModule[0])->isRotation = true;
				else ((SuperModule *)childModule[0])->isRotation = false;
			} else {
				expHeight[l] = childModule[0]->width;
				expWidth[l] = childModule[0]->height;
				if (!isRotation) ((SuperModule *)childModule[0])->isRotation = false;
				else ((SuperModule *)childModule[0])->isRotation = true;
			}
			set4ChildCordinatesSetOne(0,l,expHeight,expWidth);
			break;
		case 'b':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[1]->height;
				expWidth[l] = childModule[1]->width;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = true;
				else ((SuperModule *)childModule[1])->isRotation = false;
			} else {
				expHeight[l] = childModule[1]->width;
				expWidth[l] = childModule[1]->height;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = false;
				else ((SuperModule *)childModule[1])->isRotation = true;
			}
			set4ChildCordinatesSetOne(1,l,expHeight,expWidth);
			break;
		case 'c':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[2]->height;
				expWidth[l] = childModule[2]->width;
				if (!isRotation) ((SuperModule *)childModule[2])->isRotation = true;
				else ((SuperModule *)childModule[2])->isRotation = false;
			} else {
				expHeight[l] = childModule[2]->width;
				expWidth[l] = childModule[2]->height;
				if (!isRotation) ((SuperModule *)childModule[2])->isRotation = false;
				else ((SuperModule *)childModule[2])->isRotation = true;
			}
			set4ChildCordinatesSetOne(2,l,expHeight,expWidth);
			break;
		case 'd':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[3]->height;
				expWidth[l] = childModule[3]->width;
				if (!isRotation) ((SuperModule *)childModule[3])->isRotation = true;
				else ((SuperModule *)childModule[3])->isRotation = false;
			} else {
				expHeight[l] = childModule[3]->width;
				expWidth[l] = childModule[3]->height;
				if (!isRotation) ((SuperModule *)childModule[3])->isRotation = false;
				else ((SuperModule *)childModule[3])->isRotation = true;
			}
			set4ChildCordinatesSetOne(3,l,expHeight,expWidth);
			break;
		}
	}
}

*/
void SuperModule::set4ChildCordinatesSetOne(int curChild, int pos, int expHeight[], int expWidth[]) {
	switch(selectedPlacement) {
	case placement1:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[0]+expWidth[1],y+0);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[0]+expWidth[1]+expWidth[2],y+0);
				break;
			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]+expHeight[1]);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]+expHeight[1]+expHeight[2]);
				break;

			}
		}
		break;
	case placement2:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[0]+expWidth[1],y+0);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[0]+expWidth[1],y+expHeight[2]);
				break;

			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]+expHeight[1]);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[2],y+expHeight[0]+expHeight[1]);
				break;
			}
		}
		break;
	case placement3:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[0],y+expHeight[1]);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[0],y+expHeight[1]+expHeight[2]);
				break;

			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[1],y+expHeight[0]);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[1]+expWidth[2],y+expHeight[0]);
				break;

			}
		}
		break;
	case placement4:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+max(expWidth[0],expWidth[1]),y+0);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+max(expWidth[0],expWidth[1]),y+expHeight[2]);
				break;

			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+0,y+max(expHeight[0],expHeight[1]));
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[2],y+max(expHeight[0],expHeight[1]));
				break;

			}
		}
		break;
	case placement5:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[0],y+expHeight[1]);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[0]+expWidth[2],y+expHeight[1]);
				break;
			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[1],y+expHeight[0]);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[1],y+expHeight[0]+expHeight[2]);
				break;

			}
		}
		break;
	case placement6:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+max(expWidth[0],expWidth[1]),y+0);
				break;
			case 3:
				childModule[curChild]->setCordinates(x+expWidth[1],y+max(expHeight[0],expHeight[2]));
				break;

			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+0,y+max(expHeight[0],expHeight[1]));
				break;
			case 3:
				childModule[curChild]->setCordinates(x+max(expWidth[0],expWidth[2]),y+expHeight[1]);
				break;

			}
		}
		break;
	}
}
	
/*
void SuperModule::set3ChildCordinates() {
	int expHeight[3];
	int expWidth[3];
	
	for(int l=0; l < 3; l++){
		if(isRotation) {
			if(selectedRotation[l]=='0') selectedRotation[l]='1';
				else selectedRotation[l]='0';
		}
		switch (selectedPattern[l]) {
		case 'a':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[0]->height;
				expWidth[l] = childModule[0]->width;
				if (!isRotation) ((SuperModule *)childModule[0])->isRotation = true;
				else ((SuperModule *)childModule[0])->isRotation = false;
			} else {
				expHeight[l] = childModule[0]->width;
				expWidth[l] = childModule[0]->height;
			if (!isRotation) ((SuperModule *)childModule[0])->isRotation = false;
				else ((SuperModule *)childModule[0])->isRotation = true;
			}
			set3ChildCordinatesSetOne(0,l,expHeight,expWidth);
			break;
		case 'b':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[1]->height;
				expWidth[l] = childModule[1]->width;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = true;
				else ((SuperModule *)childModule[1])->isRotation = false;
			} else {
				expHeight[l] = childModule[1]->width;
				expWidth[l] = childModule[1]->height;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = false;
				else ((SuperModule *)childModule[1])->isRotation = true;
			}
			set3ChildCordinatesSetOne(1,l,expHeight,expWidth);
			break;
		case 'c':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[2]->height;
				expWidth[l] = childModule[2]->width;
				if (!isRotation) ((SuperModule *)childModule[2])->isRotation = true;
				else ((SuperModule *)childModule[2])->isRotation = false;
			} else {
				expHeight[l] = childModule[2]->width;
				expWidth[l] = childModule[2]->height;
				if (!isRotation) ((SuperModule *)childModule[2])->isRotation = false;
				else ((SuperModule *)childModule[2])->isRotation = true;
			}
			set3ChildCordinatesSetOne(2,l,expHeight,expWidth);
			break;
		}
	}
}
*/
void SuperModule::set3ChildCordinates() {
	int expHeight[3];
	int expWidth[3];
	
	for(int l=0; l < 3; l++){
		if(isRotation) {
			if(selectedRotation[l]=='0') selectedRotation[l]='1';
				else selectedRotation[l]='0';
		}
		switch (selectedPattern[l]) {
		case 'a':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[0]->height;
				expWidth[l] = childModule[0]->width;
				((SuperModule *)childModule[0])->isRotation = false;
			} else {
				expHeight[l] = childModule[0]->width;
				expWidth[l] = childModule[0]->height;
				((SuperModule *)childModule[0])->isRotation = true;
			}
			set3ChildCordinatesSetOne(0,l,expHeight,expWidth);
			break;
		case 'b':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[1]->height;
				expWidth[l] = childModule[1]->width;
				((SuperModule *)childModule[1])->isRotation = false;
			} else {
				expHeight[l] = childModule[1]->width;
				expWidth[l] = childModule[1]->height;
				((SuperModule *)childModule[1])->isRotation = true;
			}
			set3ChildCordinatesSetOne(1,l,expHeight,expWidth);
			break;
		case 'c':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[2]->height;
				expWidth[l] = childModule[2]->width;
				((SuperModule *)childModule[2])->isRotation = false;
			} else {
				expHeight[l] = childModule[2]->width;
				expWidth[l] = childModule[2]->height;
				((SuperModule *)childModule[2])->isRotation = true;
			}
			set3ChildCordinatesSetOne(2,l,expHeight,expWidth);
			break;
		}
	}
}


void SuperModule::set3ChildCordinatesSetOne(int curChild, int pos, int expHeight[], int expWidth[]) {
	switch(selectedPlacement) {
	case placement1:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[0]+expWidth[1],y+0);
				break;
			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]+expHeight[1]);
				break;
			}
		}
		break;
	case placement2:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[0],y+expHeight[1]);
				break;
			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			case 2:
				childModule[curChild]->setCordinates(x+expWidth[1],y+expHeight[0]);
				break;
			}
		}
		break;
	}
}
/*
void SuperModule::set2ChildCordinates() {
	int expHeight[2];
	int expWidth[2];
	
	for(int l=0; l < 2; l++){
		if(isRotation) {
			if(selectedRotation[l]=='0') selectedRotation[l]='1';
				else selectedRotation[l]='0';
		}
		switch (selectedPattern[l]) {
		case 'a':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[0]->height;
				expWidth[l] = childModule[0]->width;
				if (!isRotation) ((SuperModule *)childModule[0])->isRotation = true;
				else ((SuperModule *)childModule[0])->isRotation = false;
			} else {
				expHeight[l] = childModule[0]->width;
				expWidth[l] = childModule[0]->height;
				if (!isRotation) ((SuperModule *)childModule[0])->isRotation = false;
				else ((SuperModule *)childModule[0])->isRotation = true;
			}
			set2ChildCordinatesSetOne(0,l,expHeight,expWidth);
			break;
		case 'b':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[1]->height;
				expWidth[l] = childModule[1]->width;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = true;
				else ((SuperModule *)childModule[1])->isRotation = false;
			} else {
				expHeight[l] = childModule[1]->width;
				expWidth[l] = childModule[1]->height;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = false;
				else ((SuperModule *)childModule[1])->isRotation = true;
			}
			set2ChildCordinatesSetOne(1,l,expHeight,expWidth);
			break;
		}
	}
}*/

void SuperModule::set2ChildCordinates() {
	int expHeight[2];
	int expWidth[2];
	
	for(int l=0; l < 2; l++){
		if(isRotation) {
			if(selectedRotation[l]=='0') selectedRotation[l]='1';
				else selectedRotation[l]='0';
		}
		switch (selectedPattern[l]) {
		case 'a':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[0]->height;
				expWidth[l] = childModule[0]->width;
				((SuperModule *)childModule[0])->isRotation = false;
			} else {
				expHeight[l] = childModule[0]->width;
				expWidth[l] = childModule[0]->height;
				((SuperModule *)childModule[0])->isRotation = true;
			}
			set2ChildCordinatesSetOne(0,l,expHeight,expWidth);
			break;
		case 'b':
			if (selectedRotation[l]=='0') {
				expHeight[l] = childModule[1]->height;
				expWidth[l] = childModule[1]->width;
				if (!isRotation) ((SuperModule *)childModule[1])->isRotation = true;
				else ((SuperModule *)childModule[1])->isRotation = false;
			} else {
				expHeight[l] = childModule[1]->width;
				expWidth[l] = childModule[1]->height;
				((SuperModule *)childModule[1])->isRotation = true;
			}
			set2ChildCordinatesSetOne(1,l,expHeight,expWidth);
			break;
		}
	}
}
void SuperModule::set2ChildCordinatesSetOne(int curChild, int pos, int expHeight[], int expWidth[]) {
	switch(selectedPlacement) {
	case placement1:
		if (!isRotation) {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+expWidth[0],y+0);
				break;
			}
		} else {
			switch(pos) {
			case 0:
				childModule[curChild]->setCordinates(x+0,y+0);
				break;
			case 1:
				childModule[curChild]->setCordinates(x+0,y+expHeight[0]);
				break;
			}
		}
		break;
	}
}
void SuperModule::set1ChildCordinates() {
	if (!isRotation) ((SuperModule *)childModule[0])->isRotation = false;
	else ((SuperModule *)childModule[0])->isRotation = true;
childModule[0]->setCordinates(x+0,y+0);
}
void SuperModule::showModules()
{
  	cout << "is leaf Node : " << isLeaf << endl;
	cout << "Module Count : " << moduleCount << endl;
	cout << "Height= " << height << endl;
	cout << "Width= " << width << endl;
	cout << "---------------------------------------------------" << endl;
	//accuTotalDeadArea += (area - tempTotArea);
	//cout << "--------------------------------------------------------------------" << endl;
	//cout << "Area -----> " << area << endl;
	//cout << "Current DeadArea -----> " << area - tempTotArea  << endl;
	//cout << "Current DeadArea % ----->" << (((double)area - (double)tempTotArea)/(double)area)*100.00 << endl;
	//cout << "Accumlated DeadArea -----> " << accuTotalDeadArea  << endl;
	//cout << "Accumlated DeadArea % ----->" << ((double)accuTotalDeadArea/(double)area)*100.00 << endl;
	//cout << "--------------------------------------------------------------------" << endl;
	

}

void SuperModule::showXYModules()
{
  	cout << "is leaf Node : " << isLeaf << endl;
	cout << "Module Count : " << moduleCount << endl;
	cout << "is Rotated : " << isRotation << endl;
	cout << "x==="<< x << endl;
	cout << "y===" << y << endl;
	cout << "Height= " << height << endl;
	cout << "Width= " << width << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "selected Pattern : " << selectedPattern << endl;
	cout << "Sellected Rotation : " << selectedRotation << endl;
	cout << "selected placement : " << selectedPlacement << endl;
	cout << "---------------------------------------------------" << endl;

}

void SuperModule::showXYZModules()
{
  	cout << "is Rotated : " << isRotation << endl;
	cout << "x==="<< x << endl;
	cout << "y===" << y << endl;
	cout << "Height= " << height << endl;
	cout << "Width= " << width << endl;
}







//---------------------------------------------------------------------------
//   Auxilliary Functions
//---------------------------------------------------------------------------










void error(char *msg,char *msg2){
  printf(msg,msg2);
  cout << endl;
  throw 1;
}


double seconds(){
  /* rusage time;
   getrusage(RUSAGE_SELF,&time);
   return (double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec);*/ 
   return 1;
}








int min(int a,int b){
	return a>b?b:a;
}
int max(int a,int b){
 return a>b?a:b;
}


int max(int a,int b, int c){
 return a>b?(a>c?a:c):(b>c?b:c);
}


int max(int a,int b, int c, int d){
 //return a>b?a:b;
 return (a>b?a:b)>(c>d?c:d)?(a>b?a:b):(c>d?c:d);

}
