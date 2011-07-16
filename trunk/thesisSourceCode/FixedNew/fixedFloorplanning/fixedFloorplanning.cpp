// fixedFloorplanning.cpp : Defines the entry point for the console application.
//

#include <iostream.h>     
#include "stdio.h"
#include <string.h>
#include <time.h>
#include "floorPlanning.h"
#include <stdio.h>

/*int main(int argc, char* argv[])
{
	printf("Hello World!\n");
	return 0;
}*/

int main(int argc,char **argv)
{
   char filename[80],outfile[80]="";
  
double last_time  = 1 ;

   if(argc<=1){
     printf("Usage: fixedFloorplanning <inputFileName> <outputFileName>  \n");
     return 0;
   }else{
     int argi=1;
     if(argi < argc) strcpy(filename, argv[argi++]);
     if(argi < argc) strcpy(outfile, argv[argi++]);
   }

   try{

	   time_t startTime,stopTime,totalTime;
	   startTime = time (NULL);
     //double time = seconds();
      FloorPlan fp;
     fp.read(filename);
     //fp.show_modules();
  
     double last_time = floorPlanning(fp);
     //Random_Floorplan(fp,times);
      stopTime = time (NULL);
	 fp.list_information();

     { // log performance and quality
       if(strlen(outfile)==0)
         strcpy(outfile,strcat(filename,".res"));

        totalTime = stopTime - startTime;
       //printf("CPU time       = %.2f\n",seconds()-time);
       printf("Last CPU time  = %d\n",totalTime);
       FILE *fs= fopen(outfile,"a+");
       fprintf(fs,"File= %s, Width= %.4f, Height= %.4f, Area= %.4f,Total Area= %.4f, Dead=%.4f, Time=%d \n",
		   filename, float(fp.Width)*1e-3, float(fp.Height)*1e-3, float(fp.getArea())*1e-6,float(fp.getTotalArea())*1e-6, float(fp.getDeadSpace()), totalTime);
	//	fprintf(fs,"Cost= %.6f, Area= %.0f, Dead=%.4f \n",float(fp.getCost()), float(fp.getArea()), float(fp.getDeadSpace()));
      // 
       //fprintf(fs,"CPU= %.2f, Cost= %.6f, Area= %.0f, Dead=%.4f ", last_time, float(fp.getCost()),  float(fp.getArea()), float(fp.getDeadSpace()));
      // fprintf(fs," :%d %d %.0f \n", times, local, avg_ratio);
	    //cout << "Num of Module  = " << modules_N << endl;
		//cout << "Height         = " << Height*1e-3 << endl;
		//cout << "Width          = " << Width*1e-3 << endl;
		//cout << "Area           = " << Area*1e-6 << endl;
		//cout << "Total Area     = " << TotalArea*1e-6 << endl;
		//printf( "Dead Space     = %.2f\n", getDeadSpace());

	   //#include <time.h>

//int main ()
//{
  //time_t seconds;

  //seconds = time (NULL);
  //printf ("%ld hours since January 1, 1970", seconds/3600);
  
 // return 0;
//}

       fclose(fs);
     }

   }catch(...){}
   return 1;
}
