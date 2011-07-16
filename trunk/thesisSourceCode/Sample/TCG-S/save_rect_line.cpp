//----------------------------------------------------------------
#include <LEDA/window.h>
#include "fplan.h"
#include "stdio.h"
#include "save_rect_line.h"
//-----------------------------------------------------------------
void save_rect(char* s, Clo_Red_Graph& fp, double time)
{
  int size=fp.modules_info.size();
  int i, j;
  FILE *fs=fopen(strcat(s, ".out"), "a+");
  fprintf(fs, "%.1f\n", time); 
  fprintf(fs, "%d\n", fp.getArea());
  fprintf(fs, "%d\n", fp.getWireLength());
  fprintf(fs, "%d\n", size);

  for(i=0; i<size; i++){
   fprintf(fs, "%d %d %d %d\n", fp.modules_info[i].x,
               fp.modules_info[i].y, fp.modules_info[i].rx,
               fp.modules_info[i].ry);   
  }
  fclose(fs);
}


void save_rect_line(char* s, Clo_Red_Graph& fp, double time)
{
  int size=fp.modules_info.size();
  int i, j;
  FILE *fs=fopen(strcat(s, ".out"), "a+");
  fprintf(fs, "%.1f\n", time); 
  fprintf(fs, "%d\n", fp.getArea());
  fprintf(fs, "%d\n", fp.getWireLength());
  fprintf(fs, "%d\n", size);

  for(i=0; i<size; i++){
   fprintf(fs, "%d %d %d %d\n", fp.modules_info[i].x,
               fp.modules_info[i].y, fp.modules_info[i].rx,
               fp.modules_info[i].ry);   
  }

  fprintf(fs, "%d\n", fp.network.size());
  for(i=0; i < fp.network.size(); i++){
     fprintf(fs, "%d", fp.network[i].size());
     Pin& p0=*fp.network[i][0]; 
     int x=p0.ax;
     int y=p0.ay;
     if(fp.network[i].size()==1)
        fprintf(fs, " %d %d\n", p0.ax, p0.ay);
     else{
        fprintf(fs, " %d %d", p0.ax, p0.ay);

        for(j=1; j< fp.network[i].size(); j++){
           Pin& p=*fp.network[i][j];
           if(j!=fp.network[i].size()-1)
             fprintf(fs, " %d %d", p.ax, p.ay);
           else
             fprintf(fs, " %d %d\n", p.ax, p.ay);
        }
      }
  }  
  fclose(fs);
}
