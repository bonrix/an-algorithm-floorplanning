//---------------------------------------------------------------------------
#include <iostream>             
#include <cstring>
#include <cmath>
#include <time.h>
//#ifdef BSD
#include <sys/time.h>
#include <sys/resource.h>
//#endif
#include <LEDA/string.h>
#include "fplan.h"
#include "clo_red.h"
#include "draw_win.h"
#include "show_graph.h"
#define alpha 0.0
#define beta 1.0
//---------------------------------------------------------------------------

const float init_avg = 200;
int hill_climb_stage = 3;
float avg_ratio=20;
float Random_Floorplanning(Clo_Red_Graph &fp,int times);
float lamda=1.3;

double mean(vector<double> &chain){
  double sum=0;
  for(int i=0; i < chain.size();i++)
     sum+= chain[i];
  return sum/chain.size();
}

double std_var(vector<double> &chain){
  double m = mean(chain);
  double sum=0;
  double var;
  int N= chain.size();

  for(int i=0; i < N;i++)
     sum += (chain[i]-m)*(chain[i]-m);

  var = sqrt(sum/(N-1));
//  printf("  m=%.4f ,v=%.4f\n",m,var);

  return var;
}

/* 
   Simulated Annealing B*Tree Floorplanning
   P: probability to accept in avg. delta cost (0~1)
   r: temperature decreasing rate (0~1)
   t: terminating temperature
    conv_rate: converge condition which is maximal fail rate (0~1)
   k: factor of the number of permutation in one temperature
*/
void SA_Floorplanning(Clo_Red_Graph &fp,float P,float r,float Term_T,
	   	      float conv_rate,int k)
{
  Solution Best,E;
  int MT,uphill,reject;
  float pre_cost,cost,d_cost,reject_rate;
  int N = k * fp.size();
  float T, Actual_T=1;
  float avg=init_avg;
  float max_reject_rate = 1;
  ofstream fs("/tmp/debug");
  double rf= Random_Floorplanning(fp, k);
  double estimate_avg =  rf / avg_ratio;
  //cout << "Random Average Delta Cost = " << rf << endl;
 // cout << "Estimate Average Delta Cost = " << estimate_avg << endl;

  if(hill_climb_stage==0)
    avg = estimate_avg;

  T = avg / log(P);  
  
  // get inital solution
  fp.Compute_Cost(alpha, beta, Best);
  fp.get_solution(Best);
  pre_cost = Best.Cost;
  E = Best;

  fs.precision(4);
  cout.precision(4);
  
  int good_num=0,bad_num=0;
  float total_cost=0;
  int count=0;
  int temp_counter=0;
  do{
      count++;
      MT=uphill=reject=0;
      total_cost = 0;

      fs   << "Iteration " << count << ", T=" << Actual_T << endl;

     vector<double> chain;
     for(; uphill < N && MT < 2*N; MT++){
	fp.permutate_2();
	//fp.permutate();

         
         fp.Test();
         fp.packing();
         
         temp_counter++;
         fp.Compute_Cost(alpha, beta, Best);
         cost = fp.get_cost();
         d_cost = cost - pre_cost;
         //cout<<"d_cost="<<d_cost<<endl;
         float p = exp(d_cost/T);
         //cout<<"p="<<p<<endl;    

         chain.push_back(cost);
 
         if(d_cost <=0 || rand_01() < p ){
           //cout<<"T = "<<Actual_T<<" cost = "<<cost<<endl;
           pre_cost = cost;
           total_cost += d_cost;

           if(d_cost != 0)
       	     fs << "    total= " << total_cost << endl;

           if(d_cost > 0){ 
             uphill++;
             fs << " bad= " << d_cost << ": \t\t" << p <<  endl;
              bad_num++;
           }

 	   if(d_cost < 0){
  	     fs << " good= " << d_cost << endl;
             good_num++;
           }
           fp.get_solution(E);

           // keep best solution
           if(alpha!=0.0&&beta!=0.0){
             if((fp.getArea()<Best.Area)&&(fp.getWireLength()<Best.Wire)){
               fp.get_solution(Best);
               //cout << "   ==> < Area, Wire > = " <<  Best.Area <<
               //      ", " << Best.Wire <<" >" << endl;             
             }                      
           }else{
             if(cost < Best.Cost){
               fp.get_solution(Best);
               //cout << "   ==>  Cost= " <<  Best.Cost << endl;
              // assert(Best.Cost >= fp.getTotalArea());
             }
           }
         }
         else{
           reject++;
           fp.recover(E);
         }
     }//for_MT

     double sv = std_var(chain);
     float r_t = exp(lamda*T/sv);
     T = r_t*T;

     // After Hill-Climbing, start to use normal SA
     if(count == hill_climb_stage){
       T = estimate_avg/log(P);
       //cout<<"est T="<<T<<endl;
       Actual_T = exp(estimate_avg/T);
       //cout<<"Actual_T="<<Actual_T<<endl; 
     }
     if(count > hill_climb_stage){
       Actual_T = exp(estimate_avg/T);
       //cout<<"Actual_T="<<Actual_T<<endl;
       max_reject_rate = conv_rate;
     }

     reject_rate = float(reject)/MT;

     fs    << " reject=" << reject_rate << endl;
  }while(reject_rate < max_reject_rate && Actual_T > Term_T);

  fp.recover(Best);
  fp.Compute_Wire_Length();
}

float Random_Floorplanning(Clo_Red_Graph &fp,int times){
  Solution Best;  
  fp.packing();  
  Best.Area=fp.getArea();
  Best.Wire=fp.getWireLength();
  //show_graph(fp.getWidth(), fp.getHeight(), fp.modules_info); 
  fp.Compute_Cost(alpha, beta, Best); 
  fp.get_solution(Best);
  double total_cost=0, pre_cost, cost;
  int t = 0;
  int N = times;
  pre_cost = (double)fp.get_cost();
  //cout<<"start random"<<endl;
  for(int i=0; i < N; i++){
    //cout<<"start per"<<endl;
    fp.permutate_2();
    //fp.permutate();
    //fp.Test();
    fp.packing();
    fp.Compute_Cost(alpha, beta, Best);

    cost = fp.get_cost();
    if(cost-pre_cost > 0){
      total_cost+= (cost-pre_cost);
      t++;
      pre_cost = cost;
    }
   
    if(alpha!=0.0&&beta!=0.0){
      if((fp.getArea()<Best.Area)||(fp.getWireLength()<Best.Wire)){
        fp.get_solution(Best);
        //cout << "   ==>  Cost= " <<  Best.Cost << endl;             
      }    
    }else{
      if(cost <= Best.Cost){
        fp.get_solution(Best);
        //cout << "==> Cost=" << Best.Cost << endl;
      }
    }
  }
  fp.recover(Best);
  return (total_cost/t);
  //exit(0);
}

double seconds()
{
#ifdef BSD       // BSD system instead of SYSV
   rusage time;
   getrusage(RUSAGE_SELF,&time);
   return (double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec);
#else
   return double(clock())/CLOCKS_PER_SEC;
#endif
}


int main(int argc,char **argv)
{
   cout << "Floorplanning Version:" << TIMESTAMP << endl;
   int times=10;
   char filename[80];   
   char filename2[80]; 
   float init_temp=0.9,term_temp=0.3,temp_scale=0.9,cvg_r=0.95;
   int target_cost=-1;
   if(argc<=2){
     printf("Usage: TCGs <File for Networks> <File for Rectilinear>\n");
     printf("             [times=%d] [hill_climb_stage=%d]\n", times, hill_climb_stage);
     printf("             [avg_ratio=%d]\n",avg_ratio);
     printf("             [inital-temp=%.2f] [terminal-temp=%.2f]\n", init_temp, term_temp);
     printf("             [temp-scale=%.2f]  [convergence-ratio=%.2f\n", temp_scale, cvg_r);
     return 0;
   }else{
     int argi=1;
     if(argi < argc) strcpy(filename,argv[argi++]);
     if(argi < argc) strcpy(filename2,argv[argi++]);
     if(argi < argc) times=atoi(argv[argi++]);
     if(argi < argc) hill_climb_stage=atoi(argv[argi++]);
     if(argi < argc) avg_ratio=atof(argv[argi++]);
     if(argi < argc) target_cost=atoi(argv[argi++]); 
     if(argi < argc) init_temp=atof(argv[argi++]);
     if(argi < argc) term_temp=atof(argv[argi++]);
     if(argi < argc) temp_scale=atof(argv[argi++]);
     if(argi < argc) cvg_r=atof(argv[argi++]);
   }
   float area, wire, cost;
   double time = seconds();

   Clo_Red_Graph fp;   
   fp.read(filename);
   fp.ReadRectilinear(filename2);   
   //cout<<"start initialization"<<endl;
   //fp.init();
   fp.init_square();
   //fp.packing();      
   SA_Floorplanning(fp,init_temp,temp_scale, term_temp,cvg_r,times);
   area=fp.getArea();     
   wire=fp.getWireLength(); 
   cost = fp.get_cost();
   //cout<<"Best Area= "<< fp.getArea()<<endl;
   //cout<<"Best Wire= "<<fp.getWireLength()<<endl;
   //cout<<"Best Cost= "<<fp.get_cost()<<endl;
   //show_graph(fp.getWidth(), fp.getHeight(), fp.modules_info);
   //cout<<"Wire="<<fp.getWireLength()<<endl;
   time = seconds()-time;
  
   if(cost<=target_cost){
      drawing_rect_line(filename2, fp, time);
   }
   
//   { //log performance and quality
     FILE *fs= fopen(strcat(filename2,".res"),"a+");
     //cpu_time area times avg_ratio hill_climb_stage init_temp temp_scale
     fprintf(fs,"%.1f %.1f %.1f %d %d %d %.2f %.2f\n",time, area, wire, times, 
      hill_climb_stage, int(avg_ratio), init_temp, temp_scale);
     fclose(fs);
//   }
   return 1;
}