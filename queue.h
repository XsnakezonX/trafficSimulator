#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
/*----- structure definition -------------------------------------------------*/

struct queueStruct
{
   char                  data;
   int                   born;
   struct queueStruct   *next;
};
typedef struct queueStruct QUEUE;

/*----- function prototypes --------------------------------------------------*/

static QUEUE *newNode    (char data, int born);
extern void   push       (QUEUE **, char, int born);
extern void   pop        (QUEUE **);
extern void   printQueue (QUEUE *);
extern void   emptyQueue (QUEUE **);
