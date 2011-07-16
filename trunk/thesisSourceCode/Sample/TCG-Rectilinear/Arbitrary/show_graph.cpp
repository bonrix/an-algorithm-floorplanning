#include <LEDA/window.h>
//#include <LEDA/string.h>
#include "fplan.h"
#include "stdio.h"
#include "show_graph.h"

void  show_graph(float width, float height, Modules_Info& modules_info)
{
     
     int i, j, n;
     n=modules_info.size();
     leda_window W;
     W.set_bg_color(13);
     W.display();
     int c1;
     float s=0.4;

    if(width<height)
      s=height/W.xmax();
    else
      s=width/W.xmax();

     do{
         int area;
         int wire;
         double time;
         double max_x=0.0;
         double max_y=0.0;


         W.set_line_width(2);  
         for(i=0; i<n; i++){
            float x1, y1, x2, y2;
            x1=modules_info[i].x;
            y1=modules_info[i].y;
            x2=modules_info[i].rx;
            y2=modules_info[i].ry;

            W.draw_filled_rectangle(x1/s, y1/s, x2/s, y2/s, 0); 
            W.draw_rectangle(x1/s, y1/s, x2/s, y2/s, 1);
 
            if(max_x<double(x2/s))
              max_x=x2/s;
            if(max_y<double(y2/s))
              max_y=y2/s;
            
            j=i+1;
            
            char p[2];
            char p1[2];
            int l=0;
            int k;
            do{
                k=j%10;
            
                switch (k){
                  case 0:
                    p[l++]='0'; 
                    break; 
                  case 1:
                    p[l++]='1';     
                    break; 
                  case 2:
                    p[l++]='2';
                    break;
                  case 3:
                    p[l++]='3'; 
                    break;  
                  case 4:
                    p[l++]='4';
                    break; 
                  case 5:
                    p[l++]='5'; 
                    break; 
                  case 6:
                    p[l++]='6'; 
                    break; 
                  case 7:
                    p[l++]='7'; 
                    break; 
                 case 8:
                    p[l++]='8'; 
                   break; 
                 case 9:
                    p[l++]='9'; 
                   break;            
                }
             }while(j=j/10);//do
             
             k=l-1;
             for(j=0; j < l; j++)
             {
             	p1[j]=p[k--];
             }
             if (i>=9)
             	W.text_box(x1/s, x2/s, y2/s, p1); 	  
             else
             	W.text_box(x1/s, x2/s, y2/s, p1[0]); 	   	
         }//for

         W.set_line_width(5);
         W.draw_segment(0.0, max_y, max_x, max_y);
         W.draw_segment(max_x, 0.0, max_x, max_y);
         W.draw_segment(0.0, 0.0, max_x, 0);
         W.draw_segment(0.0, 0.0, 0, max_y);

         int c1=W.read_mouse();
   
         if(c1==MOUSE_BUTTON(1))
           s=s-0.05;
         else if(c1==MOUSE_BUTTON(3))
           s=s+0.05;
    
        W.clear();
     }while(!W.ctrl_key_down());

}