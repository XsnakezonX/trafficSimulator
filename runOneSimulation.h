#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "queue.h"

/*a struct approach to obtaining results from each simulation*/
struct resultArray{
   float res1;
   float res2;
   float res3;
   float res4;
   float res5;
   float res6;
   float res7;
   float res8;
};
typedef struct resultArray RESULT;
 
/*define my own boolean operator*/
#define TRUE 1
#define FALSE 0
typedef char BOOLEAN; /*use char for smallest memory space*/


/*function prototypes*/
static RESULT *makeResultArray(float , float , float , float , float , float , float , float );
extern void runOneSimulation(RESULT **,const gsl_rng *,float , int , float , int );
extern char oppositeValue(char );
extern void stdOut(int , int , int , int , float , float , float , float , float , float , float , float );
