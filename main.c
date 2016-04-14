/***************************************************************************************************/
// MAIN.C
// This is where the preprocessing and branching takes place
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include "header.h"

/******** CONFIG VARIABLES START ****************/
//Stores the number to vertices
int gNoOfVertex = 100;
//Stores the desity of graph in percentage
int gGraphDensity = 10;
//The default log level is ERR
int gLogLevel = LERROR;
/************ CONFIG VARIABLES END **************/

//The output is stored here. For simplicity take static memory and that too for
//the largest possible dist[] matrix
int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

//The mode of the program
PRG_MODE_E gProgramMode = INVALID_MODE;

void printProgUsage()
{
	printf("\nUsage: ./a.out -[r|i[s|b|f [fileName]]\n"
			"\t-r\t\t\tRandom Mode\n"
			"\t-is\t\t\tInteractive Mode - User Input - Simple Scheme Application\n"
			"\t-ib\t\t\tInteractive Mode - User Input - Binomial Heap Application\n"
			"\t-if\t\t\tInteractive Mode - User Input - Fibonacci Heap Application\n"
			"\t-i[s|b|f] [fileName]\tInteractive Mode - File Input\n\n");
}

int main(int argc, char *argv[])
{
	char *cpFileName = NULL_PTR;
	int retVal;

	/* Scan the user arguments and determine the mode */
	if (argc < MIN_ARGS)
	{
		printProgUsage();
		return PRG_ERR;
	}

	switch (argc)
	{
		case 2:
		case 3:
		{
			/* Parse the second argument */
			if ('r' == argv[1][1])
			{
				myLog(INFO, "Random Mode");
				gProgramMode = RANDOM_MODE;
			}
			else if ('i' == argv[1][1])
			{
				if ('s' == argv[1][2])
				{
					myLog(INFO, "Interactive Mode - Simple");
					gProgramMode = INTERACTIVE_SIMPLE_MODE;
				}
				else if ('b' == argv[1][2])
				{
					myLog(INFO, "Interactive Mode - Binomial");
					gProgramMode = INTERACTIVE_BINOMIAL_MODE;
				}
				else if ('f' == argv[1][2])
				{
					myLog(INFO, "Interactive Mode - Fibonacci");
					gProgramMode = INTERACTIVE_FIBONACCI_MODE;
				}
				else
				{
					myLog(ERROR, "Invalid Mode!");
					printProgUsage();
					return PRG_ERR;
				}
			}
			else
			{
				myLog(ERROR, "Invalid Mode!");
				printProgUsage();
				return PRG_ERR;
			}

			/* Parse file name if present */
			if (argc >= 3)
			{
				cpFileName = argv[2];
				myLog(INFO, "File name: [%s]\n", cpFileName);
				
				/* Make all mode into corresponding file mode */
				gProgramMode += 3;
			}
			
			/* Change the log level. Secret functionality :) Disabled currently */
			if (4 == argc)
			{
				if (((argv[3][0] - '0') >= 0)
					&& ((argv[3][0] - '0') <= 2))
				gLogLevel = (argv[3][0] - '0');
			}
			
			break;
		}
		default:
		{
			printProgUsage();
			return PRG_ERR;
		}
	}
	
	/* Invoke respective functions */
	switch (gProgramMode)
	{
		case RANDOM_MODE:
		{
			retVal = randomProc();
			break;
		}
		case INTERACTIVE_SIMPLE_MODE:
		{
			retVal = simpleProc();
			break;
		}
		case INTERACTIVE_BINOMIAL_MODE:
		{
			retVal = binoProc();
			break;
		}
		case INTERACTIVE_FIBONACCI_MODE:
		{
			retVal = fiboProc();
			break;
		}
		case INTERACTIVE_SIMPLE_FILE_MODE:
		{
			retVal = simpleFileProc(cpFileName);
			break;
		}
		case INTERACTIVE_BINOMIAL_FILE_MODE:
		{
			retVal = binoFileProc(cpFileName);
			break;
		}
		case INTERACTIVE_FIBONACCI_FILE_MODE:
		{
			retVal = fiboFileProc(cpFileName);
			break;
		}
		default:
		{
			myLog(ERROR, "Invalid Mode: [%d]", gProgramMode);
		}
	}
	
	if (OK != retVal)
	{
		myLog(ERROR, "Proc function failed for mode: [%d]", gProgramMode);
		return PRG_ERR;
	}

	return PRG_OK;
}
