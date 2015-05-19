#ifndef __CPABE
#define __CPABE

#define MR_PAIRING_BN

#include "LSSS.h"
#include "pairing_3.h"

G2 hashStrToG2(char*,G2,PFC&);

struct Waters_CPABECipherText
{
	LSSSPolicy policy;
	GT CT;
	G2 C_prime;
	G2* C;
	G1* D;
	
	Waters_CPABECipherText(LSSSPolicy policy,GT CT,G2 C_prime,G2* C,G1* D):policy(policy),CT(CT),C_prime(C_prime),C(C),D(D){};
};
class Waters_CPABEPublicKey
{
	public:
		PFC& pfc;
		G1 g1,g1_a;
		G2 g2,g2_a;
		GT egg_alpha;
		
	public:
		Waters_CPABEPublicKey(PFC& pfc,G1 g1,G1 g1_a,G2 g2,G2 g2_a,GT egg_alpha):
		pfc(pfc),g1(g1),g1_a(g1_a),g2(g2),g2_a(g2_a),egg_alpha(egg_alpha){};
		
		Waters_CPABECipherText encrypt(GT M,LSSSPolicy policy,Big s=0);
		
		friend class Waters_CPABEMasterKey;
};
class Waters_CPABESecretKey
{
	public:
		PFC& pfc;
		size_t attrCnt;
		char** attrs;
		G1 K,L;
		G2* Kx;
	public:
		Waters_CPABESecretKey(PFC& pfc,char** attrs,G1 K,G1 L,G2* Kx,size_t attrCnt): pfc(pfc), attrs(attrs), K(K), L(L), Kx(Kx),attrCnt(attrCnt) {};
	
		GT decrypt(Waters_CPABECipherText,int*);
		
};
class Waters_CPABEMasterKey
{
	public:
		PFC& pfc;
		G1 g1_alpha;
	public:
		Waters_CPABEMasterKey(PFC& pfc,G1 g1_alpha): pfc(pfc),g1_alpha(g1_alpha) {};
		
		Waters_CPABESecretKey genSecretKey(char** attrs,size_t attrCnt,Waters_CPABEPublicKey);
};

#endif
