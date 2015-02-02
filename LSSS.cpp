#include "dealerlessIBE.h"
#include "LSSS_err.h"

template<class SecretType,class ShareType>
LSSS()
{
	policySet = false;
	secretSet = false;
}

template<class SecretType,class ShareType>
int LSSS::setPolicy(LSSSPolicy* policy)
{
	int err = SETPLYBASE;
	this.policy.rowCnt = policy->rowCnt;
	this.policy.colCnt = policy->colCnt;
	
	if(policy->rowCnt>0 && policy->colCnt>0)
	{
		this.policy.A = policy->A;
		this.policy.labels = policy->labels;
		
		policySet = true;
	}
	else
	{
		err = SETPLYERR;
	}
	
	return err;
}

template<class SecretType,class ShareType>
int LSSS::setSecret(SecretType secret)
{
	int err = SETSRTBASE;
	if(!this.policySet)
	{
		err = SETSRTPLY;
	}
	else
	{
		this.r = (SecretType*)malloc(this.policy.rowCnt * sizeof(SecretType));
		r[0] = secret;
		secretSet = true;
	}
	return err;
}

template<class SecretType,class ShareType>
int genShares(ShareType** shares,size_t* shareCnt)
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
		_shares = (ShareType*)malloc(policy.rowCnt * sizeof(ShareType));
		for(i=0;i<rowCnt;i++)
		{
			SecretType tmp;
			for(j=0;j<colCnt;j++)
			{
				tmp += r[j] * policy[i][j];
			}
			shares[i] = tmp;
		}
		*shares = _shares;
	}
	return err;
}