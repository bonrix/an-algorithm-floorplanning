#ifndef floorPlanH
#define floorPlanH

//---------------------------------------------------------------------------

//#include <cstdio>
//---------------------------------------------------------------------------
#include <vector>
//#include <string.h>
#include <iostream> // standard streams
#include <fstream> // file streams
#include <sstream> // string streams
using namespace std;
#include <map>



struct Pin{
  int mod;
  int net;
  int x,y;    // relative position
  int ax,ay;  // absolute position
  Pin(int x_=-1,int y_=-1){ x=x_,y=y_; }
};
typedef vector<Pin> Pins;
typedef Pin* Pin_p;
typedef vector<Pin_p> Net;
typedef vector<Net > Nets;

enum Module_Type { MT_Hard, MT_Soft, MT_Reclinear, MT_Buffer };

class Module{
public:
  int id;
  char name[20];
  int width,height;
  int x,y;
  int area;
  Pins pins;
  Module_Type type;
  void setCordinates( int cx, int cy) {x= cx,y=cy;}
  
};

static int minTotalArea = 0;
static int accuTotalDeadArea = 0;
static int finalWidth,finalHeight;

typedef vector<Module> Modules;

static char modulePattern44[24][5] = { "abcd", "abdc", "acbd","acdb", "adbc", "adcb", 
    								   "bacd", "badc", "bcad", "bcda", "bdac", "bdca", 
									   "cabd", "cadb", "cbad", "cbda", "cdab", "cdba",
									   "dabc", "dacb", "dbac", "dbca", "dcab", "dcba" };
static char moduleRoatation44[16][5] = { "0000", "0001", "0010","0011", "0100", "0101", "0110","0111",
										 "1000", "0001", "1010","1011", "1100", "1101", "1110","1111"};
static char modulePattern43[6][4] = { "abc", "acb", "bac", "bca", "cab", "cba" };
static char moduleRoatation43[8][4] = { "000", "001", "010","011", "100", "101", "110","111"};
static char modulePattern42[2][3] =  { "ab", "ba"};
static char moduleRoatation42[4][3] = { "00", "01", "10", "11" };

enum placementType { placement1, placement2, placement3, placement4, placement5, placement6};

class SuperModule;

typedef vector<SuperModule> SuperModules;

void setInOrder(SuperModules &superModules);

class SuperModule : public Module {
	private:
		int moduleCount;
		Module* childModule[4];
		bool isLeaf;


 	

		
		char selectedPattern[5];
		char selectedRotation[5];
		placementType selectedPlacement;
		
	public:

		bool isRotation;
		void setChild(Modules &modules, int intialPosition, int childCount, bool isItLeaf);
		void setChildComposite(SuperModules &superModules, int intialPosition, int childCount, bool isItLeaf);
		void arrangeBlocks();
		void setChildCordinates();
		void showModules();
		void showXYModules();
		void showXYZModules();
		

		
	
	private:
		void arrange4Blocks();
		void arrange3Blocks();
		void arrange2Blocks();
		void arrange1Blocks();
		void set4ChildCordinates();
		void set3ChildCordinates();
		void set2ChildCordinates();
		void set1ChildCordinates();
		void set4ChildCordinatesSetOne(int curChild, int Pos,int expHeight[], int expWidth[]);
		void set3ChildCordinatesSetOne(int curChild, int Pos,int expHeight[], int expWidth[]);
		void set2ChildCordinatesSetOne(int curChild, int Pos,int expHeight[], int expWidth[]);


};



struct Module_Info{
  bool rotate, flip;
  int x,y;
  int rx,ry;
};

typedef vector<Module_Info> Modules_Info;

class FloorPlan{
  public:
    FloorPlan();
    void read(char*);
  
  
    double getCost();    

    int    size()         { return modules_N; }
    double getTotalArea() { return TotalArea; }
    double getArea()      { return Area;      }
    double getWidth()     { return Width;     }
    double getHeight()    { return Height;    }
    float  getDeadSpace();

    // information
    void list_information();
    void show_modules();    
    void normalize_cost(int);
	void setInOrder();
    
  public:
    void clear();
   
    double Area;
    double Width,Height;
   
    double TotalArea;
    
    int modules_N;    
    Modules modules;
    Module  root_module;
    Modules_Info modules_info;    
   
    double norm_area, norm_wire;
    
    
  private:

	void read_dimension(Module&);
    void read_IO_list(Module&,bool parent);
    void read_network();
   map<string,int> net_table;
    string filename; 

};


void error(char *msg,char *msg2="");
double seconds();
int min(int x,int y);
int max(int x,int y);
int max(int x,int y, int z);
int max(int x,int y, int z, int a);


      
#endif
