//----------------------------------------------------------------
#include <LEDA/window.h>
#include "fplan.h"
#include "stdio.h"
#include "draw_win.h"
//-----------------------------------------------------------------

void drawing_rect(char* s, Clo_Red_Graph& fp, double time)
{
  leda_window W;
  int size=fp.modules_info.size();
  int i, j;

  FILE *fs=fopen(strcat(s, ".out"), "a+");
  fprintf(fs, "%.1f\n", time); 
  fprintf(fs, "%.1f\n", fp.getArea());
  fprintf(fs, "%.1f\n", fp.getWidth());
  fprintf(fs, "%.1f\n", fp.getHeight());
  fprintf(fs, "%f\n", fp.getWireLength());
  fprintf(fs, "%d\n", size);
  for(i=0; i<size; i++){
    fprintf(fs, "%.1f %.1f %.1f %.1f\n", fp.modules_info[i].x,
               fp.modules_info[i].y, fp.modules_info[i].rx,
               fp.modules_info[i].ry);     
  }

  fprintf(fs, "%d\n", fp.network.size()); 
  for(i=0; i < fp.network.size(); i++){
     fprintf(fs, "%d", fp.network[i].size()); 
     for(j=0; j< fp.network[i].size(); j++){
        Pin& p=*fp.network[i][j];
        if(j!=fp.network[i].size()-1)
           fprintf(fs, " %f %f", p.ax, p.ay);
        else
           fprintf(fs, " %f %f\n", p.ax, p.ay);    
      }
  }  
  fclose(fs);
}

void drawing_rect_line(char* s, Clo_Red_Graph& fp, double time)
{
  int size=fp.modules_info.size();
  int i, j;
  FILE *fs=fopen(strcat(s, ".out"), "a+");
  fprintf(fs, "%.1f\n", time); 
  fprintf(fs, "%.1f\n", fp.getArea());
  fprintf(fs, "%.1f\n", fp.getWidth());
  fprintf(fs, "%.1f\n", fp.getHeight());
  fprintf(fs, "%.1f\n", fp.getWireLength());
  fprintf(fs, "%d\n", size);
  for(i=0; i<size; i++){
   fprintf(fs, "%.1f %.1f %.1f %.1f\n", fp.modules_info[i].x,
               fp.modules_info[i].y, fp.modules_info[i].rx,
               fp.modules_info[i].ry);   
  }

  fprintf(fs, "%d\n", fp.network.size());
  for(i=0; i < fp.network.size(); i++){
     fprintf(fs, "%d", fp.network[i].size());
     Pin& p0=*fp.network[i][0]; 
     float x=p0.ax;
     float y=p0.ay;
     if(fp.network[i].size()==1)
        fprintf(fs, " %.1f %.1f\n", p0.ax, p0.ay);
     else
        fprintf(fs, " %.1f %.1f", p0.ax, p0.ay);
     for(j=1; j< fp.network[i].size(); j++){
        Pin& p=*fp.network[i][j];
        if(j!=fp.network[i].size()-1)
           fprintf(fs, " %.1f %.1f", p.ax, p.ay);
        else
           fprintf(fs, " %.1f %.1f\n", p.ax, p.ay);

       x=p.ax;
       y=p.ay;    
      }
  }  
  fclose(fs);

}