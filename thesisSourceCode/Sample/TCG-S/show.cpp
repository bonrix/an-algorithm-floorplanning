//---------------------------------------------------------------------------
#include <iostream>             
#include <cstring>
#include <cmath>
#include <LEDA/window.h>
//---------------------------------------------------------------------------

int main(int argc, char** argv)
{
     if(argc!=3){
          cout<<"please enter <show> <filename> <a--area, w--wire>"<<endl;
          exit(0);
     }

     char filename[40];
     strcpy(filename, argv[1]);
     char c=*argv[2];
     int i, j, n;
     leda_window W;
     W.set_bg_color(13);
     //W.set_bg_color(0);
     W.set_line_width(1);
     W.display();
     int c1;
     double s=50.0;
     do{
         int area;
         int wire;
         double time;
         float max_x=0.0;
         float max_y=0.0;

         FILE *fs= fopen(filename,"r");
         fscanf(fs, "%f\n", &time);
         fscanf(fs, "%d\n", &area);
         fscanf(fs, "%d\n", &wire);
         fscanf(fs, "%d\n", &n);
         for(i=0; i<n; i++){
            int x1, y1, x2, y2;
            fscanf(fs,"%d %d %d %d\n", &x1, &y1, &x2, &y2);
            W.draw_filled_rectangle(x1/s, y1/s, x2/s, y2/s, 0); 
            //W.draw_rectangle(x1/s, y1/s, x2/s, y2/s, 1);
            W.draw_segment(x1/s, y1/s, x1/s, y2/s, 1);
            W.draw_segment(x1/s, y1/s, x2/s, y1/s, 1);
            W.draw_segment(x1/s, y2/s, x2/s, y2/s, 1);
            W.draw_segment(x2/s, y1/s, x2/s, y2/s, 1); 

            if(max_x< (x2/s))
              max_x=x2/s;
            if(max_y< (y2/s))
              max_y=y2/s;  
         }//for
         W.set_line_width(3);
         W.draw_segment(0.0, max_y, max_x, max_y);
         W.draw_segment(max_x, 0.0, max_x, max_y);
         W.draw_segment(0.2, 0.0, 0.2, max_y);
         W.draw_segment(0.0, 0.3, max_x, 0.3);
         W.set_line_width(1);

         if(c=='w'){
            int p_x1, p_y1, p_x2, p_y2;
            fscanf(fs, "%d\n", &n);
            for(i=0; i<n; i++){
              int n1;
              fscanf(fs, "%d", &n1); 
              fscanf(fs, "%d %d", &p_x1, &p_y1);
              for(j=1; j<n1; j++){
                 fscanf(fs, " %d %d", &p_x2, &p_y2);
                 W.draw_segment(p_x1/s, p_y1/s,  p_x2/s, p_y2/s);
                 p_x1=p_x2;
                 p_y1=p_y2;         
              }//for_j
            }//for_i
         }//if
         fclose(fs);
         int c1=W.read_mouse();
   
         if(c1==MOUSE_BUTTON(1)&&s>=20)
           s=s-10;
         else if(c1==MOUSE_BUTTON(3))
           s=s+10;
    
         W.clear();
      }while(!W.ctrl_key_down());
     //int c1;
     //c1=getchar();
     return 1;
}