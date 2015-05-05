#include "LSSS.h"
#include <sstream>

//#define DEBUG
#ifdef DEBUG
#include <cstdio>
#endif

int gcd(int a,int b)
{
	if((a%b)*(b%a)==0)
		return (a%b)+(b%a);
	return gcd(a%b,b%a);
}

// rowSummand = rowSummand*mulS + rowAddend*mulA
void rowAdd(int* rowSummand,int mulS,int* rowAddend,int mulA,size_t colCnt)
{
	int i;
	for(i=0;i<colCnt;i++)
		rowSummand[i] = rowSummand[i]*mulS + rowAddend[i]*mulA;
}

int calcW(LSSSPolicy policy,char** shareLabels,size_t shareCnt,int* w,int* t)
{
	int err = RECONSTRBASE;
	int** GJmatrix;
	int** originMatrix;
	int i,j;
	int* shareIndex = (int*)calloc(shareCnt,sizeof(int));
	for(i=0;i<shareCnt;i++)
	{
		shareIndex[i] = i;
	}
	
	GJmatrix = (int**)calloc(shareCnt,sizeof(int*));
	for(i=0;i<shareCnt;i++)
	{
		GJmatrix[i] = (int*)calloc(shareCnt,sizeof(int));
		for(j=0;j<shareCnt;j++)
		{
			GJmatrix[i][j]=0;
		}
		GJmatrix[i][i] = 1;
	}
	
	originMatrix = (int**)calloc(shareCnt,sizeof(int*));
	for(i=0;i<shareCnt;i++)
	{
		originMatrix[i] = (int*)calloc(policy.colCnt,sizeof(int));
		for(j=0;j<policy.rowCnt;j++)
		{
			if(strcmp(shareLabels[i],policy.labels[j])==0)
			{
				memcpy(originMatrix[i],policy.A[j],policy.colCnt*sizeof(int));
				break;
			}
		}
		if(j==policy.rowCnt)
		{
			//err = RECONSTRLABEL;
			memset(originMatrix[i],0,policy.colCnt*sizeof(int));
		}
	}
	
	// GJ elimination
	if(err == RECONSTRBASE)
	{
		for(i=0;i<policy.colCnt;i++)
		{
#ifdef DEBUG
			printf("origin\tGJ\n");
			for(int k=0;k<shareCnt;k++)
			{
				for(int l=0;l<policy.colCnt;l++)
					printf(" %d",originMatrix[k][l]);
				printf("\t");
				for(int l=0;l<shareCnt;l++)
					printf(" %d",GJmatrix[k][l]);
				printf("\n");
			}
#endif				
			for(j=i;j<shareCnt && originMatrix[j][i]==0;j++);
			
			if(j==shareCnt)
				continue;
			
			if(j!=i) //swap row
			{
				int* tmpRow;
				//Share<SecretType> tmpSrt;
				int tmpIndex;
				
				tmpRow = originMatrix[i];
				originMatrix[i] = originMatrix[j];
				originMatrix[j] = tmpRow;
				
				tmpRow = GJmatrix[i];
				GJmatrix[i] = GJmatrix[j];
				originMatrix[j] = tmpRow;
				
				//tmpSrt = shares[i];
				//shares[i] = shares[j];
				//shares[j] = tmpSrt;
				tmpIndex = shareIndex[i];
				shareIndex[i] = shareIndex[j];
				shareIndex[j] = tmpIndex;
			}
			
			int scalarI = originMatrix[i][i];
			int scalarJ;
			
			for(j=0;j<shareCnt;j++)
			{
				if(originMatrix[j][i]==0 || j==i)
					continue;
				
				scalarJ = -originMatrix[j][i];
				rowAdd(originMatrix[j],scalarI,originMatrix[i],scalarJ,shareCnt);
				rowAdd(GJmatrix[j],scalarI,GJmatrix[i],scalarJ,shareCnt);
			}
		}
#ifdef DEBUG
			printf("origin\tGJ\n");
			for(int k=0;k<shareCnt;k++)
			{
				for(int l=0;l<policy.colCnt;l++)
					printf(" %d",originMatrix[k][l]);
				printf("\t");
				for(int l=0;l<shareCnt;l++)
					printf(" %d",GJmatrix[k][l]);
				printf("\n");
			}
#endif
		if(originMatrix[0][0] == 0)
			err = RECONSTRELIMAT;
	}
	
	if(err == RECONSTRBASE)
	{
		for(i=1;i<shareCnt && originMatrix[0][i]==0;i++);
		if(i!=shareCnt)
			err = RECONSTRELIMAT;
	}
	
	if(err == RECONSTRBASE)
	{
		//*secret = 0;
		for(i=0;i<shareCnt;i++)
		{
			//*secret += shares[i].share * GJmatrix[0][i];
			w[shareIndex[i]] = GJmatrix[0][i];
		}
		//*secret /= originMatrix[0][0];
		*t = originMatrix[0][0];
	}
	
	for(i=0;i<shareCnt;i++)
	{
		free(originMatrix[i]);
		free(GJmatrix[i]);
	}
	free(originMatrix);
	free(GJmatrix);
	free(shareIndex);
	
	return err;
}

// LSSSPolicy

int LSSSPolicy::toString(char*,size_t)
{
	return 0;
}

int LSSSPolicy::toBinary(byte*,size_t)
{
	return 0;
}

// Share

template<class SecretType>
int Share<SecretType>::toString(char* buf,size_t)
{
	ostringstream out;
	out<<share;
	strcpy(buf,out.str().c_str());
	return strlen(buf);
}

template<class SecretType>
int Share<SecretType>::toBinary(byte*,size_t)
{
	return 0;
}

template<>
int Share<Big>::toString(char* buf,size_t sz)
{
	big x;
	int len,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	x = this->share.getbig();
	
	len=cotstr(x,mip->IOBUFF);
	
	if(sz < len)
		return -1;
	for (i=0;i<len;i++) 
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++)='\0';
	
	return ptr-buf;
}

// LSSS

template<class SecretType>
LSSS<SecretType>::LSSS(LSSSPolicy policy) : policy(policy), secretSet(false){}

//LSSS<GT>::LSSS(LSSSPolicy policy,PFC pfc) : policy(policy), secretSet(false),pfc(pfc){}

template<class SecretType>
int LSSS<SecretType>::setR(SecretType* r)
{
	int err = SETSRTBASE;
	if(!r)
	{
		err = SETSRTNULL;
	}
	else
	{
		this->r = r;
		secretSet = true;
	}
	return err;
}

template<class SecretType>
int LSSS<SecretType>::genShares(Share<SecretType>** shares,size_t* shareCnt)
{
	int err = GENSHRBASE;
	int i,j;
	if(!secretSet)
	{
		err = GENSHRSRT;
	}
	else
	{
		Share<SecretType>* _shares = new Share<SecretType>[policy.rowCnt];
		
		for(i=0;i<policy.rowCnt;i++)
		{
			SecretType tmp = 0;     //*****TODO
			for(j=0;j<policy.colCnt;j++)
			{
				tmp += r[j] * policy.A[i][j];
			}
			_shares[i].share = tmp;
			_shares[i].label = policy.labels[i];
		}
		*shares = _shares;
		*shareCnt = policy.rowCnt;
	}
	return err;
}

template<class SecretType>
int LSSS<SecretType>::reconstructSecret(Share<SecretType>* shares,size_t shareCnt,SecretType* secret)
{
	char** shareLabels = (char**)calloc(shareCnt,sizeof(char*));
	int* w = (int*)calloc(shareCnt,sizeof(int));
	int t;
	for(int i=0;i<shareCnt;i++)
	{
		shareLabels[i] = shares[i].label;
	}
	
	int err = calcW(policy,shareLabels,shareCnt,w,&t);
	if(err == RECONSTRBASE)
	{
		*secret = 0;
		for(int i=0;i<shareCnt;i++)
			*secret += shares[i].share * w[i];
		*secret /= t;
	}
	
	return err;
}

// Template class definition

template class LSSS<int>;
template class LSSS<Big>;

//template class LSSS<GT>;
//template class LSSS<ECn>;

//template class Share<Big>;
//template class Share<ECn>;
