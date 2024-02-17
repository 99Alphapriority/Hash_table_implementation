#include<math.h>

#include "prime.h"

/*******************************************************************************
Function Name:	isPrime
Arguments:	integer number
Description:	
		Checks whether the input integer is prime or not
Retrun value:	integer
*******************************************************************************/
int isPrime(const int x)
{
	if(x < 2) 
		return -1;	//undefined
	if(x < 4)
		return 1;
	if(0 == (x%2))
		return 0;
	
	for(int i = 3; i <= floor(sqrt((double) x)); i += 2)
	{
		if(0 == (x%i))
			return 0;
	}
	
	return 1;
}

/******************************************************************************
Function Name:	nextPrime
Arguments:	integer number
Description:
		Returns the next prime number after x, or x if x is prime
Return value:	integer
******************************************************************************/
int nextPrime(int x)
{
	while(1 != isPrime(x))
		x++;
	return x;
}
