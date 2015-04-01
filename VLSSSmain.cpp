#include "LSSS.h"
#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;

Miracl precision = 20;
ECn g;

int loadCurve()
{
	ifstream common("../lib/miracl/common.ecs");
    int bits,ep;
    miracl *mip=&precision;

    ECn W;
    Big a,b,p,q,x,y,d;
    long seed;

    common >> bits;
    mip->IOBASE=16;
    common >> p >> a >> b >> q >> x >> y;
    mip->IOBASE=10;
	
	ecurve(a,b,p,MR_PROJECTIVE);

    if (!g.set(x,y))
    {
        cout << "Problem - point (x,y) is not on the curve" << endl;
        return 1;
    }

    W=g;
    W*=q;

    if (!W.iszero())
    {
        cout << "Problem - point (x,y) is not of order q" << endl;
        return 2;
    }
	return 0;
}

LSSSPolicy getPolicy()
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
	return policy;
}

int main()
{
	int err;
	
	if(loadCurve())
		return 0;
	
	ECGroup G(g);
	VLSSS<Big,ECGroup> vlsss(getPolicy(),G);
	
	cout<<"VLSSS init."<<endl;
	
	Big secretVector[3] = {100,3,4};
	err = vlsss.setR(secretVector);
	
	ECGroup W = _pow(G,secretVector[0]);
	char buf[100];
	W.toString(buf,100);
	//cout<<buf<<endl;
	//cout<<"sizeof(ECGroup) = "<<sizeof(ECGroup)<<endl;
	
	cout<<"VLSSS R set. err = "<<err<<endl;
	
	ECGroup* tokens;
	size_t tokenCnt;
	err = vlsss.getVerifiableTokens(&tokens,&tokenCnt);
	
	cout<<tokenCnt<<" Tokens Gen. err = "<<err<<endl;
	//cout<<tokens<<endl;
	for(int i=0;i<tokenCnt;i++)
	{
		cout<<tokens[i].p<<endl;
	}
	
	Share<Big>* shares;
	size_t shareCnt;
	vlsss.genShares(&shares,&shareCnt);
	
	cout<<shareCnt<<" Shares Gen."<<endl;
	for(int i=0;i<shareCnt;i++)
	{
		cout<<shares[i].label<<' '<<shares[i].share<<endl;
	}
	
	for(int i=0;i<shareCnt;i++)
	{
		if(vlsss.verify(tokens,tokenCnt,shares[i]))
		{
			cout<<"True"<<endl;
		}
		else
		{
			cout<<"False"<<endl;
		}
	}
	
	return 0;
}
