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
#include "save_rect_line.h"
#include "show_graph.h"
//---------------------------------------------------------------------------
const float init_avg = 200;
int hill_climb_stage = 4;
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
  //cout << "Estimate Average Delta Cost = " << estimate_avg << endl;

  if(hill_climb_stage==0)
    avg = estimate_avg;

  T = avg / log(P);  
  //cout<<"T="<<T<<endl;  
  
  // get inital solution
  fp.get_solution(Best);
  pre_cost = Best.Cost;
  //cout<<"pre_cost="<<pre_cost<<endl;
  E = Best;

  fs.precision(4);
  //cout.precision(4);
  
  int good_num=0,bad_num=0;
  float total_cost=0;
  int count=0;

  do{
      count++;
      MT=uphill=reject=0;
      total_cost = 0;

      fs   << "Iteration " << count << ", T=" << Actual_T << endl;
      cout << "Iteration " << count << ", T=" << Actual_T << endl;

     vector<double> chain;
     for(; uphill < N && MT < 2*N; MT++){

         fp.perturbate2();    
         //fp.Test();
         fp.contour_packing();
         fp.compute_wire_length();
         //fp.compute_cost(alpha, beta, Best);
         cost = fp.get_cost();
         d_cost = cost - pre_cost;
         float p = exp(d_cost/T);

         chain.push_back(cost);
 
         if(d_cost <=0 || rand_01() < p ){
           cout<<"==> cost="<<cost<<endl;
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
           if(cost < Best.Cost){
             fp.get_solution(Best);
             //show_graph(fp, fp.getWidth(), fp.getHeight(), fp.modules_info); 
           }
         }
         else{
           reject++;
           fp.recover(E);
         }
     }//for_MT
     //T = r*T;
     //cout<<"decreasing T="<<T<<endl;
     double sv = std_var(chain);
     float r_t = exp(lamda*T/sv);
     T = r_t*T;

     // After Hill-Climbing, start to use normal SA
     if(count == hill_climb_stage){
       T = estimate_avg/log(P);
       Actual_T = exp(estimate_avg/T);
     }
     if(count > hill_climb_stage){
       Actual_T = exp(estimate_avg/T);
       max_reject_rate = conv_rate;
     }

     reject_rate = float(reject)/MT;

     fs    << " reject=" << reject_rate << endl;
  }while(reject_rate < max_reject_rate && Actual_T > Term_T);

  fp.recover(Best);
  fp.compute_wire_length();
}

float Random_Floorplanning(Clo_Red_Graph &fp,int times){
  Solution Best;  
  fp.contour_packing();
  fp.compute_wire_length();  
  Best.Area=fp.getArea();
  fp.get_solution(Best);
  double total_cost=0, pre_cost, cost;
  int t = 0;
  int N = times;
  pre_cost = (double)fp.get_cost();

  //cout<<"start random"<<endl;
  for(int i=0; i < N; i++){
    fp.perturbate1();
    //cout<<"start contour_packing"<<endl;
    //fp.Test();
    fp.contour_packing();
    fp.compute_wire_length();
    //fp.compute_cost(alpha, beta, Best);

    cost = fp.get_cost();
    if(cost-pre_cost > 0){
      total_cost+= (cost-pre_cost);
      t++;
      pre_cost = cost;
    }
   
    if(cost <= Best.Cost){
      fp.get_solution(Best);
      //cout << "==> Cost=" << Best.Cost << endl;
    }
  }
  fp.recover(Best);
  //cout<<"Area="<<fp.getArea();
  //cout<<"Wire="<<fp.getWireLength();
  return (total_cost/t);
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
   char filename1[80];
   float init_temp=0.9,term_temp=0.3,temp_scale=0.9,cvg_r=0.95;
   int target_cost=-1;
   if(argc<=1){
     printf("Usage: TCGs <filename> [times=%d] [hill_climb_stage=%d]\n",
           times, hill_climb_stage);
     printf("             [avg_ratio=%d]\n",avg_ratio);
     printf("             [inital-temp=%.2f] [terminal-temp=%.2f]\n",
	   init_temp, term_temp);
     printf("             [temp-scale=%.2f]  [convergence-ratio=%.2f\n",
	   temp_scale, cvg_r);
     return 0;
   }else{
     int argi=1;
     if(argi < argc) strcpy(filename,argv[argi++]);
     if(argi < argc) times=atoi(argv[argi++]);
     if(argi < argc) hill_climb_stage=atoi(argv[argi++]);
     if(argi < argc) avg_ratio=atof(argv[argi++]);
     if(argi < argc) target_cost=atoi(argv[argi++]); 
     if(argi < argc) init_temp=atof(argv[argi++]);
     if(argi < argc) term_temp=atof(argv[argi++]);
     if(argi < argc) temp_scale=atof(argv[argi++]);
     if(argi < argc) cvg_r=atof(argv[argi++]);
   }
   float area, wire;
   double time_s = seconds();

   Clo_Red_Graph fp;
   fp.read(filename);
   time_t t;
   srand((unsigned) time(&t)); 
   //fp.init_sqr();
   fp.init();
   SA_Floorplanning(fp,init_temp,temp_scale, term_temp,cvg_r,times);
   area=fp.getArea();
   wire=fp.getWireLength();     
   cout<<"Best Wire="<< fp.getWireLength()<<endl;
   //show_graph(fp, fp.getWidth(), fp.getHeight(), fp.modules_info);
   cout<<"Wire="<<fp.getWireLength()<<endl;
   time_s = seconds()-time_s;
   strcat(filename,"_wire");
   strcpy(filename1, filename);
   if(wire<=(float)target_cost){
      save_rect_line(filename1, fp, time_s);
   }
   
//   { //log performance and quality
     FILE *fs= fopen(strcat(filename,".res"),"a+");
     //cpu_time area times avg_ratio hill_climb_stage init_temp temp_scale
      fprintf(fs,"%.1f %.1f %.1f %d %d %d %.2f %.2f\n",time_s, area, wire, times, 
      hill_climb_stage, int(avg_ratio), init_temp, temp_scale);
     fclose(fs);
//   }
   return 1;
}