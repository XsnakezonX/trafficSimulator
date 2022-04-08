#include "queue.h"

/*----- queue functions ------------*/

/* create a new node from the given data and return a pointer to it */
static QUEUE *newNode (char data, int born)
{
   QUEUE *q = (QUEUE *)malloc(sizeof(QUEUE));
   if ( q==NULL )
   {
      /*error message for failing to allocate memory*/
      fprintf(stderr,"Insufficient memory\n");
      exit(-1);
   }
   q->data = data;
   q->born = born;
   q->next = NULL;
   return q;
}

/* push/add a value into the front of the queue */
extern void push (QUEUE **queue, char data, int born)
{
   QUEUE   *q = newNode(data,born);
   q->next = *queue;
   *queue = q;
}

/*return the born time of the last item*/
extern int bornOfPop(QUEUE **queue){
   QUEUE *q = *queue;
   int born = 0;
   if(*queue == NULL)
   {  /*error message for popping an empty queue*/
      fprintf(stderr,"Warning: attempting to pop from an empty queue.\n");
      fprintf(stderr,"Warning: line %d, file %s\n",__LINE__,__FILE__);
      return born;
   }
   while(q->next != NULL){
      q = q->next;
   }
   born = q->born;
   return born;
}

/* pop/remove an item off from the front of the queue and return the value */
extern void pop(QUEUE **queue)
{
   QUEUE *temp = *queue;

   if(*queue == NULL)
   {  /*error message*/
      fprintf(stderr,"Warning: attempting to pop from an empty queue.\n");
      fprintf(stderr,"Warning: line %d, file %s\n",__LINE__,__FILE__);
      return;
   }
   /*printf("if: %c\n");*/
   /*removing the last item in the queue in a way that prevent segmentation fault*/
   if (temp->next == NULL){

      *queue = temp->next;
      free(temp);
      return;
   }
   
   /*find the second last item in the linked list*/
   while(temp->next->next != NULL)
   {
      temp = temp->next;
   }

   /*free the memory space of the next item to remove the last item of a linked list*/
   free(temp->next);
   /*set the next pointer points to nothing*/
   temp->next = NULL;
   
   /*return data;*/
   return;
}

/* recursively print out the contents of the queue for testing purposes*/
extern void printQueue (QUEUE *queue)
{
   if ( queue==NULL )
   {
      return;
   }
   else
   {
      printf("(%c | %d)\n",queue->data,queue->born);
      printQueue(queue->next);
   }
}

/* recursively empty the queue, thus freeing all the memory */
extern void emptyQueue (QUEUE **queue)
{
   if ( *queue==NULL )
        {
      return;}
   else
   {
      pop(queue);
      emptyQueue(queue);
   }
}
