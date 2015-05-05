#include <iostream>
#include <ctime>
#include <cstdlib>

#define MR_PAIRING_BN    // AES-128 or AES-192 security
#define AES_SECURITY 128

#include "pairing_3.h"
#include "CPABE.h"

//#define DEBUG

using namespace std;

//Miracl precision = 20;

int main()
{
	cout<<"Init.."<<endl;
	
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

//	Setup

	PFC pfc(AES_SECURITY);  // initialise pairing-friendly curve
	//miracl *mip=get_mip();
	
	cout<<"Setup..."<<endl;
	Big alpha,a,s;
	G2 g2,g2_a;
	G1 g1,g1_alpha,g1_a;
	GT egg_alpha;
	
	pfc.random(alpha);
	pfc.random(a);
	pfc.random(g1);
	pfc.random(g2);
	//a = 1;		//****** DEBUG
	//alpha = 1;	//****** DEBUG
	//pfc.precomp_for_mult(g1);
	//pfc.precomp_for_mult(g2);
	
	g1_a = pfc.mult(g1,a);
	g2_a = pfc.mult(g2,a);
	g1_alpha = pfc.mult(g1,alpha);
	
	egg_alpha = pfc.power(pfc.pairing(g2,g1),alpha);
	
	cout << "a\t=" << a <<endl;
	cout << "alpha\t=" << alpha <<endl;
	cout << "g1\t=" << g1.g <<endl;
	cout << "g2\t=" << g2.g <<endl;
	cout << "g1^a\t=" << g1_a.g <<endl;
	cout << "g2^a\t=" << g2_a.g <<endl;
	cout << "e(g1,g2)^alpha = "<< egg_alpha.g <<endl;
	cout << "g1^alpha\t=" << g1_alpha.g <<endl;
	
	cout<<"Setup master key"<<endl;
	Waters_CPABEMasterKey msk(pfc,g1_alpha);
	cout<<msk.g1_alpha.g<<endl;
	
	cout<<"Setup public key"<<endl;
	Waters_CPABEPublicKey pk(pfc,g1,g1_a,g2,g2_a,egg_alpha);
	cout<<pk.g1.g<<endl;
	cout<<pk.g2.g<<endl;
	cout<<pk.g1_a.g<<endl;
	cout<<pk.g2_a.g<<endl;
	cout<<pk.egg_alpha.g<<endl;
	cout<<endl;
	
//	Encrypt
	cout<<"Encrypt..."<<endl;
	GT M(1);
	
	Waters_CPABECipherText ct = pk.encrypt(M,policy);
	
	cout<<"CT = "<<endl;
	cout<<"    C  = "<<ct.CT.g<<endl;
	cout<<"    C' = "<<ct.C_prime.g<<endl;
	for(int i=0;i<policy.rowCnt;i++)
	{
		cout<<"    C["<<i<<"] "<<ct.C[i].g<<endl;
		cout<<"    D["<<i<<"] "<<ct.D[i].g<<endl;
	}
	
	cout<<endl;
	
//	KeyGen
	cout<<"Generate sk..."<<endl;
	Waters_CPABESecretKey sk = msk.genSecretKey(policy.labels,3,pk);
	cout<<"\tK = "<<sk.K.g<<endl;
	cout<<"\tL = "<<sk.L.g<<endl;
	for(int i=0;i<sk.attrCnt;i++)
	{
		cout<<"\t"<<sk.attrs[i]<<":   ";
		cout<<"Kx["<<i<<"] = "<<sk.Kx[i].g<<endl;
	}
	cout<<endl;
	
//	Decrypt
	cout<<"Decrypt..."<<endl;
	int err;
	GT M_prime = sk.decrypt(ct,&err);

	if(!err)
	{
		cout<<"Decrypt success!!"<<endl;
	}
	else
	{
		cout<<"Decrypt fail!!"<<endl;
	}
	cout<<"M  = "<<M.g<<endl;
	cout<<"M' = "<<M_prime.g<<endl;
	
	cout << "Exit." <<endl;
	
#ifdef DEBUG
	GT mask = ct.CT;

	cout<<"----- check point -----"<<endl;
	cout<<"mask = "<<mask.g<<endl;
	cout<<"e(msk,CD) = "<<(pfc.pairing(ct.C_prime,msk.g1_alpha)).g<<endl;
	cout<<"----- ----- ----- -----"<<endl<<endl;

	cout<<"----- check point -----"<<endl;
	cout<<"K = "<<sk.K.g<<endl;
	cout<<"msk + aL = "<<(msk.g1_alpha + pfc.mult(sk.L,a)).g<<endl;
	cout<<"----- ----- ----- -----"<<endl<<endl;
	
	cout<<"----- check point -----"<<endl;
	cout<<"mask = "<<mask.g<<endl;
	cout<<"e(K-aL,C') = "<<(pfc.pairing(ct.C_prime,sk.K)/pfc.pairing(ct.C_prime,pfc.mult(sk.L,a))).g<<endl;
	cout<<"----- ----- ----- -----"<<endl<<endl;
	
	cout<<"----- check point -----"<<endl;
	for(int i=0;i<ct.policy.rowCnt;i++)
		cout<<ct.policy.labels[i]<<endl;
	for(int i=0;i<sk.attrCnt;i++)
		cout<<sk.attrs[i]<<endl;
	cout<<"----- ----- ----- -----"<<endl<<endl;
	
	cout<<"----- check point -----"<<endl;
	GT tmp1,tmp2,tmp3;
	tmp1 = pfc.power(pfc.pairing(ct.C[0],sk.L)*pfc.pairing(sk.Kx[0],ct.D[0]),3);
	tmp2 = pfc.power(pfc.pairing(ct.C[1],sk.L)*pfc.pairing(sk.Kx[1],ct.D[1]),-3);
	tmp3 = pfc.power(pfc.pairing(ct.C[2],sk.L)*pfc.pairing(sk.Kx[2],ct.D[2]),1);
	cout<<"mask = "<<mask.g<<endl;
	cout<<"calc = "<<(tmp1*tmp2*tmp3).g<<endl;
	cout<<"calc = "<<(tmp3*tmp2*tmp1).g<<endl;
	cout<<"----- ----- ----- -----"<<endl<<endl;
#endif
	
	return 0;
}
