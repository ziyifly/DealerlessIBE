#include "LSSS.h"
#include "LSSS_err.h"

int LSSSPolicy::toString(char*,size_t)
{
	return 0;
}

int LSSSPolicy::toBinary(byte*,size_t)
{
	return 0;
}

template<class SecretType>
int Share<SecretType>::toString(char*,size_t)
{
	return 0;
}

template<class SecretType>
int Share<SecretType>::toBinary(byte*,size_t)
{
	return 0;
}

template<class SecretType,class ShareType>
LSSS<SecretType,ShareType>::LSSS() : policySet(false), secretSet(false){}

template<class SecretType,class ShareType>
int LSSS<SecretType,ShareType>::setPolicy(LSSSPolicy policy)
{
	int err = SETPLYBASE;
	this->policy.rowCnt = policy.rowCnt;
	this->policy.colCnt = policy.colCnt;
	
	if(policy.rowCnt>0 && policy.colCnt>0)
	{
		this->policy.A = policy.A;
		this->policy.labels = policy.labels;
		
		policySet = true;
	}
	else
	{
		err = SETPLYERR;
	}
	
	return err;
}

template<class SecretType,class ShareType>
int LSSS<SecretType,ShareType>::setSecretVector(SecretType* secretVector)
{
	int err = SETSRTBASE;
	if(!secretVector)
	{
		err = SETSRTNULL;
	}
	else
	{
		this->r = secretVector;
		secretSet = true;
	}
	return err;
}

template<class SecretType,class ShareType>
int LSSS<SecretType,ShareType>::genShares(ShareType** shares,size_t* shareCnt)
{
	int err = GENSHRBASE;
	int i,j;
	if(!policySet)
	{
		err = GENSHRPLY;
	}
	else if(!secretSet)
	{
		err = GENSHRSRT;
	}
	else
	{
		ShareType* _shares = (ShareType*)malloc(policy.rowCnt * sizeof(ShareType));
		
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

template class LSSS<int,Share<int> >;
