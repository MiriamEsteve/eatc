
#include<stdlib.h>
#include<math.h>

#include "distrib.h"

// 'rep' booleano: con o sin repetición
int* GetRandomIndex(int min, int max, int num, int rep)
{
	int i, j;
	int *index;
	double dif=max-min;
	
	if(dif<=0 || num<=0 || !(index=(int*)malloc(sizeof(int)*num)))
		return NULL;

	for(i=0 ; i<num ; i++)
	{
		index[i] = (rand()/(double)RAND_MAX)*dif - min;
		if(!rep)
		{
			for(j=0 ; j<i ; j++)
				if(index[j] == index[i])
				{
					i--;
					break;
				}
		}
	}

	return index;
}
// 'rep' booleano: con o sin repetición
void SetRandomIndex(int min, int max, int num, int rep, int *index)
{
	int i, j;
	double dif=max-min;
	
	if(dif<=0 || num<=0)
		return;

	for(i=0 ; i<num ; i++)
	{
		index[i] = (rand()/(double)RAND_MAX)*dif + min;
		if(!rep)
		{
			for(j=0 ; j<i ; j++)
				if(index[j] == index[i])
				{
					i--;
					break;
				}
		}
	}
}

/**********************************************************************/
void RandUniform(double min, double max, double *arr, int n)
{
	int i;
	double dif = max-min;
	
	for(i=0 ; i<n ; i++)
		arr[i] = min + dif * (((double)rand())/(RAND_MAX));
}

void RandNormal(double med, double desv, double *arr, int n)
{
	int i;
	double x1, x2, r;
	
	for(i=0 ; i<n ; i++)
	{
		do
		{
			x1 = (2.0 * rand() / (double)RAND_MAX) - 1.0;
			x2 = (2.0 * rand() / (double)RAND_MAX) - 1.0;
			r = x1*x1 + x2*x2;
		}
		while(r>=1.0 || r==0.0);

		arr[i] = med + desv * x2 * sqrt(-2.0 * log(r) / r); // VIVA Box-Muller !!!
	}
}

void RandExpUniform(double min, double max, double *arr, int n)
{
	int i;
	double dif = max-min;

	for(i=0 ; i<n ; i++)
		arr[i] = exp(min + dif * (((double)rand())/(RAND_MAX)));
}

void RandExpNormal(double med, double desv, double *arr, int n)
{
	int i;
	double x1, x2, r;
	
	for(i=0 ; i<n ; i++)
	{
		do
		{
			x1 = (2.0 * rand() / (double)RAND_MAX) - 1.0;
			x2 = (2.0 * rand() / (double)RAND_MAX) - 1.0;
			r = x1*x1 + x2*x2;
		}
		while(r>=1.0 || r==0.0);

		arr[i] = exp(med + desv * x2 * sqrt(-2.0 * log(r) / r)); // VIVA Box-Muller !!!
	}
}
