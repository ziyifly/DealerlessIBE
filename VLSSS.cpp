#include "LSSS.h"
#include "VLSSS_err.h"

template<class SecretType,class DLType>
VLSSS<SecretType,DLType>::VLSSS(LSSSPolicy policy,DLType g,DLType n):LSSS(policy),g(g){}

template<class SecretType,class DLType>
int VLSSS<SecretType,DLType>::getVerifiableTokens(DLType** tokens,size_t* sz)
{
	int err = GETTOKBASE;
	if(!secretSet)
	{
		err = GETTOKR;
	}
	else
	{
		*sz = policy.colCnt;
		*tokens = (DLType*)calloc(*sz,sizeof(DLType));
		for(int i=0;i<*sz;i++)
		{
			(*tokens)[i] = pow(g,r[i],n);
		}
	}
	return err;
}

template<class SecretType,class DLType>
bool VLSSS<SecretType,DLType>::verify(Share<SecretType> share)
{
	int rowNum = -1;
	for(int i=0;i<policy.rowCnt;i++)
	{
		if(strcmp(policy.labels[i],share.label)==0)
		{
			rowNum = i;
			break;
		}
	}
	DLType innerProduct(0);
	for(int i=0;i<policy.colCnt;i++)
	{
		innerProduct = (innerProduct+_pow(g,policy.A[rowNum][i]));
	}
	return innerProduct == _pow(g,share.share);
}

template class VLSSS<int,IntGroup>;
template class VLSSS<Big,BigGroup>;