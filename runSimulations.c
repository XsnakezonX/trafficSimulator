#include "runSimulations.h"
 
int main (int argc, char *argv[])
{
   /*input validation*/
   if (argc != 5){

      fprintf(stderr,"Error: Invalid number of arguments.\n");
      fprintf(stdout,"Usage: runSimulation [1] [2] [3] [4]\n[-1.left queue rate from 0-100%] \n[-2.left queue traffic period >1 (integer)] \n[-3.right queue rate from 0-100%] \n[-4.right queue traffic period >1 (integer)]\n");
      fprintf(stdout,"Example:\n\nrunSimulation 20 5 20.2 5\n\n");
      exit(-1);
   }
   
   if((*argv[1] == '0') || (*argv[2] == '0') || (*argv[3] == '0') || (*argv[4] == '0')){
      fprintf(stderr,"Error: Invalid input. All inputs must be a non-zero value!\n");
      exit(-1);
    }
   
   /*pass data from arguments*/

   /*left*/
   float left_arrivalRate = (float)atoi(argv[1]); /*chances of car arrivaling*/
   int left_light_period = atoi(argv[2]); /*light switches every given iteration*/

   /*right*/
   float right_arrivalRate =(float)atoi(argv[3]); /*20% chances of car arrivaling*/
   int right_light_period = atoi(argv[4]); /*light switches every 50 iteration*/
    
   /*Catch negative values*/
   if((left_arrivalRate < 0) || (left_light_period < 0) || (right_arrivalRate < 0) || (right_light_period < 0)){
      fprintf(stderr,"Error: Invalid input. All inputs must be a non-zero value!\n");
      exit(-1);
   }
    
   /*example left*/
   /*float left_arrivalRate = 20.0;*/ /*20% chances of car arrivaling*/
   /*int left_light_period = 50;*/ /*light switches every 50 iteration*/

   /*example right*/
   /*float right_arrivalRate =20.0;*/ /*20% chances of car arrivaling*/
   /*int right_light_period = 50;*/ /*light switches every 50 iteration*/

    
   const gsl_rng_type *T;
   gsl_rng *r;
   /* create a random number generator */
   gsl_rng_env_setup();
   T = gsl_rng_default;
   r = gsl_rng_alloc(T);
    
   /* generate random seed */
   gsl_rng_set(r,time(0));
   
   /*data storage for the 100 run:*/

   /*---Left queue:---*/

   /*--average counter--*/
   float l_vehicles = 0;
   
   /*--measure in time unit--*/
   float l_aWait = 0;
   float l_maxWait = 0;
   float l_clearTime = 0;

   /*---Right queue:---*/

   /*--average counter--*/
   float r_vehicles = 0;
   /*--measure in time unit--*/
   float r_aWait = 0 ;
   float r_maxWait = 0;
   float r_clearTime = 0;

   
    /*under a loop*/
   int i;
   for (i=0; i<100; i++){

      RESULT *result = (RESULT *)malloc(sizeof(RESULT));
      if (result == NULL){
         fprintf(stderr,"Memory allocation failed.\n");
         exit(-1);
      }

      runOneSimulation(&result,r,left_arrivalRate,left_light_period,right_arrivalRate,right_light_period);

      /*record results for this single run*/
      l_vehicles += result->res1;
      l_aWait += result->res2;
      l_maxWait += result->res3;
      l_clearTime += result->res4;
      r_vehicles += result->res5;
      r_aWait += result->res6;
      r_maxWait += result->res7;
      r_clearTime += result->res8;
        
      /*free memory after each run and after the results are recorded*/
      free(result);
    }

   /*average number of vehicles: */
   float l_numV = (float)l_vehicles /100;

   /*average waiting time of each car: */
   float l_avgWaitTime = (float)l_aWait / 100;

   /*average maximum waiting time of each car: */
   float l_avgMaxWaitTime = (float)l_maxWait / 100;

   /*average clearance time of a queue: */
   float l_avgClearTime = (float)l_clearTime / 100;

   /*average number of vehicles: */
   float r_numV = (float)r_vehicles /100;

   /*average waiting time of each car: */
   float r_avgWaitTime = (float)r_aWait / 100;

   /*average maximum waiting time of each car: */
   float r_avgMaxWaitTime = (float)r_maxWait / 100;

   /*average clearance time of a queue: */
   float r_avgClearTime = (float)r_clearTime / 100;

   /*print results:*/
   stdOut(left_arrivalRate,left_light_period,right_arrivalRate,right_light_period,l_numV,l_avgWaitTime,l_avgMaxWaitTime,l_avgClearTime,r_numV,r_avgWaitTime,r_avgMaxWaitTime,r_avgClearTime);

   /*free memory*/
   gsl_rng_free(r);


   return 0;
}
