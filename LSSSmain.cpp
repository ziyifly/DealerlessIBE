#include "LSSS.h"
#include <cstdio>

int main()
{
	LSSSPolicy policy;
	
	policy.rowCnt = 4;
	policy.colCnt = 3;
	
	policy.A = (int**)malloc(4*sizeof(int*));
	for(int i=0;i<4;i++)
	{
		policy.A[i] = (int*)malloc(3*sizeof(int));
		for(int j=0;j<3;j++)
		{
			policy.A[i][j] = (j==0?1:policy.A[i][j-1]*(i+1));
		}
	}
	
	policy.labels = (char**)malloc(4*sizeof(char*));
	for(int i=0;i<4;i++)
	{
		policy.labels[i] = (char*)malloc(10*sizeof(char));
		sprintf(policy.labels[i],"label%d",i);
	}
	
	LSSS<int> lsss (policy);
	
	int secretVector[3] = {100,3,4};
	lsss.setR(secretVector);
	
	Share<int>* shares;
	size_t shareCnt;
	lsss.genShares(&shares,&shareCnt);
	
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<3;j++)
		{
			printf(" %d",policy.A[i][j]);
		}
		printf("\n");
	}	
	
	for(int i=0;i<shareCnt;i++)
	{
		printf("%s : %d\n",shares[i].label,shares[i].share);
	}
	
	printf("----- Reconstruct -----\n");
	
	Share<int> reShares[3] = {shares[0],shares[3],shares[1]};
	int reSrt;
	int err = lsss.reconstructSecret(reShares,3,&reSrt);
	
	for(int i=0;i<3;i++)
	{
		printf("%s : %d\n",reShares[i].label,reShares[i].share);
	}
	printf("err = %d\n",err);
	printf("reSrt = %d\n",reSrt);
	
	
	return 0;
}