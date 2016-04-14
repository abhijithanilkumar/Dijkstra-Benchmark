/***************************************************************************************************/
// FIBO.H
// Contains enums and macros for fibonacci scheme
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include "header.h"
#include <malloc.h>

/* Fibonacci Heaps are collection of trees not neccesarily binomial
The insert can be done in O(1) time
Meld can be done in O(1) time
Remove min can be done in O(n) time -- Amortized is O(log(n))
Decrease key can be done in O(n) time -- Amortized is O(1) */

#define MAX_FIBO_DEGREE 500

/* Node defination of a Fibonacci Tree */
typedef struct FiboHeap
{
	int key; //for us the key is edge cost
	int vertex; //from this we can derive (sourceVertex, vertex) = key;
	int degree; // No of children it has
	int childCut; //True if it has lost a child since it became a child of someone. Undefined for root
	struct FiboHeap *parent; // For root its NULL
	struct FiboHeap *child; // Points only to any one child
	struct FiboHeap *prev; //Useful for doubly circular linked list. Points to itself if alone
	struct FiboHeap *next; //Useful for doubly circular linked list. Points to itself if alone
}FiboHeap;
