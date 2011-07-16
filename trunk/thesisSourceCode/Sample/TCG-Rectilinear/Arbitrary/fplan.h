//---------------------------------------------------------------------------
#ifndef fplanH
#define fplanH
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <cstdio>
//---------------------------------------------------------------------------
using namespace std;

struct Pin{
  int mod;
  int net;
  float x,y;    // relative position
  float ax,ay;  // absolute position
  Pin(float x_=-1.0, float y_=-1.0){ x=x_,y=y_; }
};
typedef vector<Pin> Pins;
typedef Pin* Pin_p;
typedef vector<Pin_p> Net;
typedef vector<Net > Nets;

struct Submodule
{
  int id;
  float relation1;
  float relation2;  
};

enum Module_Type { MT_Hard, MT_Soft, MT_ReclinearL, MT_ReclinearT, MT_Buffer };

struct Module{
  int id;
  char name[20];
  float width,height;
  float x,y;
  float area;
  Pins pins;
  Module_Type type;
  vector<Submodule> submodules;
};

typedef vector<Module> Modules;

struct Module_Info{
  float x,y;
  float rx,ry;
  float width, height;
  //'0' denote 0 degree, '1' denote 90 degree
  //'2' denote 180 degree, '3' denote 270 degree 
  int degree;
  int relation;   
};

typedef vector<Module_Info> Modules_Info;

class FloorPlan{
  public:
    FloorPlan() {};
    ~FloorPlan() {};
    
    void read(char*);
    void ReadRectilinear(char*);
    void show_modules();
    void show_modules_pins();
    virtual void packing() {}

    // information
    int size() { return modules_N; }
    float getTotalArea() { return TotalArea; }
    float getArea() { return Area; }
    float getWidth() { return Width; }
    float getHeight() { return Height; }
    float getDeadSpace() { return DeadSpace; }
    float getWireLength() { return Wire; }
    
    Modules modules, Pmodules;
    Module  root_module;
    Modules_Info modules_info;  
    int Num_of_Hard; 
    int Num_of_L; 
    Nets network;
    
  protected:
    map<string,int> net_table;

    float Area, Width, Height, Wire;
    float Cost, Ratio; 
    float TotalArea;
    float DeadSpace;
    int modules_N;    
    
    void rotate_module(int, float&, float&, int);

  private:
    void read_dimension(Module&);
    void Read_RDimension(Module&);
    void Compute_relation(char, int);
    void read_IO_list(Module&,bool parent);
    void read_network();
    void create_network();   
};


void error(char *msg,char *msg2="");
float rand_01();      
//---------------------------------------------------------------------------
#endif