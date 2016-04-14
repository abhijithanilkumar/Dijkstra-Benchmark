/***************************************************************************************************/
// HEADER.H
// Contains enums and macros
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#ifndef __HEADER__
#define __HEADER__ 

#define TRUE 1
#define FALSE 0

#define OK 0
#define ERR (-1)

#define PRG_OK 0
#define PRG_ERR 1
#define MIN_ARGS 2

#define NULL_PTR 0

#define INVALID_TIME (-1)
#define MSEC_PER_SEC 1000
#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC / MSEC_PER_SEC)

#define FILELINE __FILE__, __LINE__

#define MAX_EDGE_COST 1000
#define MAX_VERTEX_NUM 500
#define INFINITY (MAX_EDGE_COST * MAX_VERTEX_NUM)

#define MAX_SCHEME_RUNS 5

#define MAX(x, y) ((x) < (y) ? (y) : (x))

#define MAX_VALID_VERTICES 4
#define MAX_VALID_DENSITIES 10
#define SIMPLE_SCHEME 0
#define BINO_SCHEME 1
#define FIBO_SCHEME 2
#define TOTAL_SCHEMES 3

typedef unsigned char _UC;
typedef unsigned short _US;
typedef unsigned int _UL;

typedef enum PRG_MODE_ENUM
{
	INVALID_MODE = 0,
        RANDOM_MODE,
        INTERACTIVE_SIMPLE_MODE,
        INTERACTIVE_BINOMIAL_MODE,
        INTERACTIVE_FIBONACCI_MODE,
        INTERACTIVE_SIMPLE_FILE_MODE,
        INTERACTIVE_BINOMIAL_FILE_MODE,
        INTERACTIVE_FIBONACCI_FILE_MODE,
	TOTAL_PRG_MODES = 0xFF
}PRG_MODE_E;

typedef enum LOG_LEVELS_ENUM
{
	LDEBUG = 0,
	LINFO,
	LERROR,
	TOTAL_LOG_LEVELS
}LOG_LEVELS_E;

typedef struct AdjList
{
        int vertexNum;
        int distance;
        struct AdjList *next;
}AdjList;

#define DEBUG LDEBUG, FILELINE
#define INFO LINFO, FILELINE
#define ERROR LERROR, FILELINE

#endif
