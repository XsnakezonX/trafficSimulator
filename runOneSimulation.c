#include "runOneSimulation.h"

/*other functions*/

/*Store results value to a struct*/
static RESULT *makeResultArray(float res1, float res2, float res3, float res4, float res5, float res6, float res7, float res8){
   RESULT *r = (RESULT *)malloc(sizeof(RESULT));
   if ( r==NULL )
   {
      fprintf(stderr,"Insufficient memory\n");
      exit(-1);
   }
   r->res1 = res1;
   r->res2 = res2;
   r->res3 = res3;
   r->res4 = res4;
   r->res5 = res5;
   r->res6 = res6;
   r->res7 = res7;
   r->res8 = res8;
   return r;
};

/*switching light to opposite signal*/
extern char oppositeValue(char current_light_signal){
   if (current_light_signal == '0'){
      return '1';
   }
   else if (current_light_signal == '1'){
      return '0';
   }
}

/*stdout stream results*/
extern void stdOut(int in1, int in2, int in3, int in4, float res1, float res2, float res3, float res4, float res5, float res6, float res7, float res8){
   fprintf( stdout, "Parameter values:\n");
   fprintf( stdout, "   from left:\n");
   fprintf( stdout, "      traffic arrival rate: %d\n",in1);
   fprintf( stdout, "      traffic light period: %d\n",in2);
   fprintf( stdout, "   from right:\n");
   fprintf( stdout, "      traffic arrival rate: %d\n",in3);
   fprintf( stdout, "      traffic light period: %d\n",in4);
   fprintf( stdout, "Results ( averaged over 100 runs ):\n");
   fprintf( stdout, "   from left:\n");
   fprintf( stdout, "      number of vehicles: %.3f\n",res1);
   fprintf( stdout, "      average waiting time: %.3f\n",res2);
   fprintf( stdout, "      maximum waiting time: %.3f\n",res3);
   fprintf( stdout, "      clearance time: %.3f\n",res4);
   fprintf( stdout, "   from right:\n");
   fprintf( stdout, "      number of vehicles: %.3f\n",res5);
   fprintf( stdout, "      average waiting time: %.3f\n",res6);
   fprintf( stdout, "      maximum waiting time: %.3f\n",res7);
   fprintf( stdout, "      clearance time: %.3f\n",res8);
}

/*para holds result array, pointer to random generator + 4 inputs*/
extern void runOneSimulation(RESULT **resultArray, const gsl_rng *r, float lQueue_rate, int lLight_period, float rQueue_rate, int rLight_period){
   /*printf("runOneSimulation\n"); test code*/
   
   int iterate = 0; /*Time unit*/

   /*instantiates left queue & right queue*/
   QUEUE   *leftQueue = NULL;
   QUEUE   *rightQueue = NULL;

   /*--counter variables for single simulator--*/
   /*left queue variables*/
   float l_numV = 0;
   int l_waitTime = 0;
   
   int l_maxWaitTime = 0;
   int l_clearTime0 = 0; /*holds the iteration number when the queue stop adding vehicle */
   int l_clearTime1 = 0; /*holds the iteration number when the queue is free of vehicle */
   

   /*right queue variables*/
   float r_numV = 0;
   int r_waitTime = 0;
   
   int r_maxWaitTime = 0;
   int r_clearTime0 = 0;
   int r_clearTime1 = 0;
   

   /*initiate traffic light's state*/
   char l_trafficLight = '0';
   char r_trafficLight = '1';

   /*Counter for starting timer*/
   BOOLEAN l_startTimer = FALSE;
   BOOLEAN l_Timer_used = FALSE;
   BOOLEAN r_startTimer = FALSE;
   BOOLEAN r_Timer_used = FALSE;

   /*to inform whether if there should be traffic activity or just light change in each iteration/time period*/
   BOOLEAN lightChanged = FALSE;

   while (!((leftQueue == NULL && rightQueue == NULL) && (iterate > 500) && (l_Timer_used == TRUE) && (r_Timer_used == TRUE))){

      /*reset lightChanged so to potentially allow traffic activities*/
      lightChanged = FALSE;

      
      /*start clearance timer for either queue after 500 iteration*/
      if ((iterate > 500)  && (l_startTimer == FALSE) && (l_Timer_used == FALSE)){
         /*printf("leftQueueTimer start at: %d \n",iterate); /*test code*/
         l_clearTime0 = iterate; /*value at this moment only once */
         l_startTimer = TRUE;
         l_Timer_used = TRUE;
      }
      if ((iterate > 500)  && (r_startTimer == FALSE) && (r_Timer_used == FALSE)){
         /*printf("rightQueueTimer start at: %d \n",iterate); /*test code*/
         r_clearTime0 = iterate; /*value at this moment only once */
         r_startTimer = TRUE;
         r_Timer_used = TRUE;
      }
      /*stop timer for one of the queue as the other queue is clearing*/
      if ((iterate > 500) && (leftQueue == NULL) && (l_startTimer == TRUE)){
         /*printf("leftQueueTimer stop1 at: %d \n",iterate); /*test code*/
         l_clearTime1 = iterate; /*value at this moment only once */
         l_startTimer = FALSE;
      }
      if ((iterate > 500) && (rightQueue == NULL) && (r_startTimer == TRUE)){
         /*printf("rightQueueTimer stop1 at: %d \n",iterate); /*test code*/
         r_clearTime1 = iterate; /*value at this moment only once */
         r_startTimer = FALSE;
      }

      /*traffic lights settings*/
      /*'0' means green; '1' means red*/

      if (iterate % lLight_period == 0){
         l_trafficLight = oppositeValue(l_trafficLight); /*switch light colour*/
         lightChanged = TRUE; /*so no traffic operation from all the queues*/
         

         /*Special feature to speed up traffics*/
         /*not need to wait if there is no car on the opposite*/
         /*if ((rightQueue == NULL) && (l_trafficLight == '1')){
            l_trafficLight = '0';
            lightChanged = 'F'; 
         }*/
         /*so traffic can still move*/
      }
      if (iterate % rLight_period == 0){
         r_trafficLight = oppositeValue(r_trafficLight); /*switch light colour*/
         lightChanged = TRUE; /*so no traffic operation from all the queues*/

         /*Special feature to speed up traffics*/
         /*not need to wait if there is no car on the opposite*/
         /*if ((leftQueue == NULL) && (r_trafficLight == '1')){
            r_trafficLight = '0';
            lightChanged = FALSE; 
            }*/
            /*so traffic can still move*/
      }
      /*deal with traffic lights collision when left & right light has different light switching period/rates*/
      /*whilst both queues are occupied*/
      if ((leftQueue != NULL) && (rightQueue != NULL) && (l_trafficLight == r_trafficLight)){
         lightChanged = TRUE;
         /*the queue with longer current average waiting gets green*/
         if ((l_waitTime/l_numV) >= (r_waitTime/r_numV)){
            l_trafficLight = '0';
            r_trafficLight = '1';
         }
         else if ((l_waitTime/l_numV) <= (r_waitTime/r_numV)){
            l_trafficLight = '1';
            r_trafficLight = '0';
         }
      }
      /*allow traffic activities when there is no lights changed*/
      if (lightChanged == FALSE){
         
         if(iterate<=500){
            /*left enqueue settings*/
            int ranNumLeft = gsl_ran_flat(r,1,100);
            if ((ranNumLeft <= lQueue_rate)){ /*rate is a int between 1-100*/
               push(&leftQueue,'C',iterate);
            }
         }
         
         /*shared temperary variable as only one queue can dequeue car at a time*/
         int tempWaitTime = 0;
         int timeAtLeft1 = iterate;
         int timeAtBorn0 = 0;
         /*left dequeue settings*/
         if (l_trafficLight == '0'){
            if (leftQueue != NULL){
               /*get the time at which the car is born*/
               timeAtBorn0 = bornOfPop(&leftQueue); 
               /*calcute the number of iteration waited*/
               tempWaitTime = timeAtLeft1 -timeAtBorn0; 

               /*check if this vehicle has the longest waiting time in this queue*/
               if (tempWaitTime > l_maxWaitTime){
                  l_maxWaitTime = tempWaitTime;
               }
               /*add this vehicle waiting time to the total left queue waiting time variable*/
               l_waitTime += tempWaitTime;
               /*increment the number of vehicle in left queue*/
               l_numV += 1;
               /*remove vehicle from the queue*/
               pop(&leftQueue);
               
            }
         }
         /*same settings for right queue*/
         /*right enqueue settings*/
         if(iterate<=500){
            int ranNumRight = gsl_ran_flat(r,1,100);
            if ((ranNumRight <= rQueue_rate)){ /*rate is a int between 1-100*/
               push(&rightQueue,'C',iterate);
            }
         }
         

         /*right dequeue settings*/
         if (r_trafficLight == '0'){
            if (rightQueue != NULL){
               /*get the time at which the car is born*/
               timeAtBorn0 = bornOfPop(&rightQueue); 
               /*calcute the number of iteration waited*/
               tempWaitTime = timeAtLeft1 - timeAtBorn0; 

               /*check if this vehicle has the longest waiting time in this queue*/
               if (tempWaitTime > r_maxWaitTime){
                  r_maxWaitTime = tempWaitTime;
               }
               /*add this vehicle waiting time to the total right queue waiting time variable*/
               r_waitTime += tempWaitTime;
               /*increment the number of vehicle in right queue*/
               r_numV += 1;
               /*remove vehicle from the queue*/
               pop(&rightQueue);
               
            }
         }

      

      }
      /*stop timer for the remaining queue as all queue is empty, exited the loop*/
      if ((iterate > 500) && (leftQueue == NULL) && (l_startTimer == TRUE)){
         /*printf("leftQueueTimer stop2 at: %d \n",iterate); /*test code*/
         l_clearTime1 = iterate; /*value at this moment only once */
         l_startTimer = FALSE;
      }
      else if ((iterate > 500) && (rightQueue == NULL) && (r_startTimer == TRUE)){
         /*printf("rightQueueTimer stop2 at: %d \n",iterate); /*test code*/
         r_clearTime1 = iterate; /*value at this moment only once */
         r_startTimer = FALSE;
      }

      iterate++;

      
      
   }

   /*exit loop*/

   

   /*empty & free both queues*/
   emptyQueue(&leftQueue);
   emptyQueue(&rightQueue);
   free(leftQueue);
   free(rightQueue);
   
   /*calculate results*/
   float l_avgWaitTIme = (float)l_waitTime / iterate;
   float l_clearTime = l_clearTime1 - l_clearTime0;

   float r_avgWaitTIme = (float)r_waitTime / iterate;
   float r_clearTime = r_clearTime1 - r_clearTime0;


   
   /*store results of this single simulation in a struct, which now can be obtained by the caller*/
   RESULT *result = *resultArray;
   result->res1 = l_numV; /*total number of vehicles that has left the left queue*/
   result->res2 = l_avgWaitTIme; /*average waiting time of all vehicles that has left the left queue*/
   result->res3 = l_maxWaitTime; /*longest waiting time of a vehicles that has left the left queue*/
   result->res4 = l_clearTime; /*time taken to empty the left queue after 500 iteration*/
   result->res5 = r_numV; /*total number of vehicles that has left the right queue*/
   result->res6 = r_avgWaitTIme; /*average waiting time of all vehicles that has left the right queue*/
   result->res7 = r_maxWaitTime; /*longest waiting time of a vehicles that has left the right queue*/
   result->res8 = r_clearTime; /*time taken to empty the right queue after 500 iteration*/

   return;
}
