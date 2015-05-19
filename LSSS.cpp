#include "LSSS.h"
#include <sstream>

//#define DEBUG
#ifdef DEBUG
#include <cstdio>
#endif

PFC* pfc;

void setLSSSPFC(PFC* _pfc)
{
	pfc = _pfc;
}

template <class NumberType>
NumberType innerProduct(NumberType** v,int* w,size_t len,int t=1)
{
	NumberType ip = 0;
	for(int i=0;i<len;i++)
	{
		ip += *(v[i]) * w[i];
	}
	return ip/t;
}

template <>
G1 innerProduct(G1** v,int* w,size_t len,int t)
{
	G1 ip;
	for(int i=0;i<len;i++)
	{
		ip = ip + pfc->mult(*(v[i]), w[i]);
	}
	
	if(t==1)
		return ip;
	else
		return pfc->mult(ip,inverse(t,pfc->order()));
}

template <>
G2 innerProduct(G2** v,int* w,size_t len,int t)
{
	G2 ip;
	for(int i=0;i<len;i++)
	{
		ip = ip + pfc->mult(*(v[i]), w[i]);
	}
	if(t==1)
		return ip;
	else
		return pfc->mult(ip,inverse(t,pfc->order()));
}

template <>
GT innerProduct(GT** v,int* w,size_t len,int t)
{
	GT ip(1);
	for(int i=0;i<len;i++)
	{
		ip = ip * pfc->power(*(v[i]), w[i]);
	}
	if(t==1)
		return ip;
	else
		return pfc->power(ip,inverse(t,pfc->order()));
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
	/*
	int* shareIndex = new int[shareCnt];
	for(i=0;i<shareCnt;i++)
	{
		shareIndex[i] = i;
	}
	*/
	GJmatrix = new int*[shareCnt];
	for(i=0;i<shareCnt;i++)
	{
		GJmatrix[i] = new int[shareCnt];
		for(j=0;j<shareCnt;j++)
		{
			GJmatrix[i][j]=0;
		}
		GJmatrix[i][i] = 1;
	}
	
	originMatrix = new int*[shareCnt];
	for(i=0;i<shareCnt;i++)
	{
		originMatrix[i] = new int[policy.colCnt];
		for(j=0;j<policy.rowCnt;j++)
		{
			if(strcmp(shareLabels[i],policy.labels[j])==0)
			{
				for(int k=0;k<policy.colCnt;k++)
				{
					originMatrix[i][k] = policy.A[j][k];
				}
				// memcpy(originMatrix[i],policy.A[j],policy.colCnt*sizeof(int));
				break;
			}
		}
		if(j==policy.rowCnt)
		{
			for(int k=0;k<policy.colCnt;k++)
			{
				originMatrix[i][k] = 0;
			}
			//err = RECONSTRLABEL;
			//memset(originMatrix[i],0,policy.colCnt*sizeof(int));
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
			
			if(j>=shareCnt)
				continue;
			
			if(j!=i) //swap row
			{
				int* tmpRow;
				int tmpIndex;
				
				tmpRow = originMatrix[i];
				originMatrix[i] = originMatrix[j];
				originMatrix[j] = tmpRow;
				
				tmpRow = GJmatrix[i];
				GJmatrix[i] = GJmatrix[j];
				GJmatrix[j] = tmpRow;
				/*
				tmpIndex = shareIndex[i];
				shareIndex[i] = shareIndex[j];
				shareIndex[j] = tmpIndex;
				*/
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
		for(i=1;i<policy.colCnt && originMatrix[0][i]==0;i++);
		if(i!=policy.colCnt)
			err = RECONSTRELIMAT;
	}
	
	if(err == RECONSTRBASE)
	{
#ifdef DEBUG

#endif
		for(i=0;i<shareCnt;i++)
		{
			w[i] = GJmatrix[0][i];
		}
		*t = originMatrix[0][0];
	}
	
	for(i=0;i<shareCnt;i++)
	{
		delete originMatrix[i];
		delete GJmatrix[i];
	}
	delete originMatrix;
	delete GJmatrix;
	//delete shareIndex;
	
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

ostream& operator<<(ostream& out,const LSSSPolicy& policy)
{
	out << policy.rowCnt <<endl;
	for(int i=0;i<policy.rowCnt;i++)
	{
		out<<policy.labels[i]<<":";
		for(int j=0;j<policy.colCnt;j++)
		{
			out<<' '<<policy.A[i][j];
		}
		out<<endl;
	}
	out<<"--- policy end ---"<<endl;
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

template<>
int Share<G1>::toString(char* buf,size_t)
{
	ostringstream out;
	out<<share.g;
	strcpy(buf,out.str().c_str());
	return strlen(buf);
}

template<>
int Share<G2>::toString(char* buf,size_t)
{
	ostringstream out;
	out<<share.g;
	strcpy(buf,out.str().c_str());
	return strlen(buf);
}

template<>
int Share<GT>::toString(char* buf,size_t)
{
	ostringstream out;
	out<<share.g;
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
			SecretType** v1 = new SecretType*[policy.colCnt];
			for(int j=0;j<policy.colCnt;j++)
			{
				v1[j] = &r[j];
			}
			_shares[i].share = innerProduct(v1,policy.A[i],policy.colCnt,1);
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
	char** shareLabels = new char*[shareCnt];
	int* w = new int[shareCnt];
	int t;
	for(int i=0;i<shareCnt;i++)
	{
		shareLabels[i] = shares[i].label;
	}
	
	int err = calcW(policy,shareLabels,shareCnt,w,&t);
	if(err == RECONSTRBASE)
	{
		SecretType** v1 = new SecretType*[policy.colCnt];
		for(int i=0;i<shareCnt;i++)
		{
			v1[i] = &shares[i].share;
		}
		*secret = innerProduct(v1,w,shareCnt,t);
	}
	
	delete shareLabels;
	delete w;
	
	return err;
}

// Template class definition

template class LSSS<int>;
template class LSSS<Big>;

template class LSSS<G1>;
template class LSSS<G2>;
template class LSSS<GT>;
