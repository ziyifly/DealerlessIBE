#include "CPABE.h"

//#define DEBUG

#ifdef DEBUG
Big *_lambda;
GT* egg_at;
const int lambdaW[3] = {34,-102,68};
#endif

G2 hashStrToG2(char* str,G2 g2,PFC& pfc)
{
	Big h = pfc.hash_to_group(str);
#ifdef DEBUG
	cout<<"h("<<str<<") = "<<pfc.mult(g2,h).g<<endl;
#endif
	return pfc.mult(g2,h);
}

Waters_CPABESecretKey Waters_CPABEMasterKey::genSecretKey(char** attrs,size_t attrCnt,Waters_CPABEPublicKey pk)
{
	Big t;
#ifdef DEBUG
	t = 1;
#else
	pfc.random(t);
#endif
	G1 K=g1_alpha+pfc.mult(pk.g1_a,t);
	G1 L=pfc.mult(pk.g1,t);

	G2 *Kx=new G2[attrCnt];
	for(int i=0;i<attrCnt;i++)
	{
		G2 h = hashStrToG2(attrs[i],pk.g2,pfc);
		Kx[i]=pfc.mult(h,t);
	}

#ifdef DEBUG
	cout<<"\tK = "<<K.g<<endl;
	cout<<"\tL = "<<L.g<<endl;
	for(int i=0;i<attrCnt;i++)
	{
		cout<<"\t"<<attrs[i]<<":   ";
		cout<<"Kx["<<i<<"] = "<<Kx[i].g<<endl;
	}
	cout<<endl;
	egg_at = new GT(pfc.power(pfc.pairing(pk.g2_a,pk.g1),t));
#endif

	Waters_CPABESecretKey sk(pfc,attrs,K,L,Kx,attrCnt);
	return sk;
}

Waters_CPABECipherText Waters_CPABEPublicKey::encrypt(GT M,LSSSPolicy policy,Big s)
{
	int m=policy.rowCnt,d=policy.colCnt;
	Big order = pfc.order();
	
	Big *v=new Big [d];
	Big *lambda=new Big [m];
	Big *r=new Big [m];
#ifdef DEBUG
	for (int i=0;i<d;i++)
	{
		v[i] = 1;
	}
#else
	for (int i=0;i<d;i++)
	{
		pfc.random(v[i]);
	}
#endif
	
	if(s==0)
	{
		s=v[0];
	}
	else
	{
		v[0]=s;
	}

#ifdef DEBUG
	for (int i=0;i<m;i++)
	{
		r[i] = 1;
	}
#else	
	for (int i=0;i<m;i++)
	{
		pfc.random(r[i]);
	}
#endif

	G2 *C=new G2 [m];
	G1 *D=new G1 [m];

	for (int i=0;i<m;i++)
	{
		lambda[i]=0;
		for (int j=0;j<d;j++)
		{
			lambda[i]+=modmult(policy.A[i][j],v[j],order);
		}
		lambda[i]%=order;
	}
	
	GT mask = pfc.power(egg_alpha,s);
	GT CT = M * mask;
	//CT=lxor(M,pfc.hash_to_aes_key(pfc.power(eg2g1a,s)));
	G2 CD=pfc.mult(g2,s);
	for (int i=0;i<m;i++)
	{
		G2 h = hashStrToG2(policy.labels[i],g2,pfc);
		C[i]=pfc.mult(g2_a,lambda[i])+pfc.mult(h,-r[i]);
		D[i]=pfc.mult(g1,r[i]);
	}
	
	Waters_CPABECipherText ct(policy,CT,CD,C,D);
	
#ifdef DEBUG
	for (int i=0;i<d;i++)
	{
		cout << "v["<<i<<"] = " <<v[i]<<endl;
	}
	for (int i=0;i<m;i++)
	{
		cout << "lambda["<<i<<"] = " <<lambda[i]<<endl;
	}
	cout<<"mask = "<<mask.g<<endl;
	_lambda = lambda;
#endif
	return ct;
}

GT Waters_CPABESecretKey::decrypt(Waters_CPABECipherText ct,int* err)
{
	int t;
	int m = ct.policy.rowCnt, d=ct.policy.colCnt;
	Big order = pfc.order();
	
	int* w = new int[attrCnt];
	int _err = calcW(ct.policy,attrs,attrCnt,w,&t);
#ifdef DEBUG
	for(int i=0;i<attrCnt;i++)
	{
		cout << "w[" << attrs[i] <<"] = "<<w[i]<<endl;
	}
	cout<<t<<endl;
#endif
	GT M,mask(1);
	if(_err == RECONSTRBASE)
	{
		GT tmp;
		for(int i=0;i<attrCnt;i++)
		{
			for(int j=0;j<ct.policy.rowCnt;j++)
			{
				if(strcmp(attrs[i],ct.policy.labels[j]) == 0)
				{
					tmp = pfc.pairing(ct.C[j],L)*pfc.pairing(Kx[i],ct.D[j]);
					mask = mask * pfc.power(tmp,w[i]);
				}
			}
		}
		mask = pfc.power(mask,inverse(t,order));
#ifdef DEBUG
		//cout<<"egg_at^"<<1<<" = "<<pfc.power(*egg_at,1).g<<endl;
		cout<<mask.g<<endl;
		//cout<<pfc.power(tmp,w[i]).g<<endl;
#endif
		M = ct.CT / pfc.pairing(ct.C_prime,K) * mask;
		*err = 0;
	}
	else
	{
		*err = -1;
	}
	
#ifdef DEBUG
	cout<<"t = "<<t<<endl;
	for(int i=0;i<attrCnt;i++)
	{
		cout << attrs[i] << " * "<<w[i]<<endl;
	}
	Big tmp = 0;
	for(int i=0;i<attrCnt;i++)
	{
		cout<<i<<": "<<_lambda[i]<<" "<<w[i]<<' '<<modmult(_lambda[i],w[i],order)<<'('<<_lambda[i]*w[i]<<endl;
		tmp += modmult(_lambda[i],w[i],order);
	}
	tmp %= order;
	cout<<tmp<<endl;
	cout<<"lambda * w = "<<modmult(tmp,inverse(t,order),order)<<endl;
	cout<<"mask = "<<mask.g<<endl;
	
#endif

	return M;
}

