#ifndef LSSS_H
#define LSSS_H

#include "LSSS_err.h"
#include "Crypto119.h"
#include "big.h"
#include "ecn.h"

#include "DLog.h"

#include <cstring>
#include <cstdlib>

void setLSSSPFC(PFC* pfc);

struct LSSSPolicy: public Serialization
{
	int** A;
	int rowCnt;
	int colCnt;
	char** labels;
	
	int toString(char*,size_t);
	int toBinary(byte*,size_t);
	
	friend ostream& operator<<(ostream&,const LSSSPolicy&);
};

template<class SecretType>
struct Share: public Serialization
{
	char* label;
	SecretType share;
	
	int toString(char*,size_t);
	int toBinary(byte*,size_t);
};

template<class SecretType>
class LSSS
{
	protected:
		bool secretSet;
		LSSSPolicy policy;
		SecretType* r;
	public:
		LSSS(LSSSPolicy);
		int setR(SecretType*);
		int genShares(Share<SecretType>**,size_t*);
		int reconstructSecret(Share<SecretType>* shares,size_t,SecretType* secret);
};

template<class SecretType,class DLType>
class VLSSS : public LSSS<SecretType>
{
	private:
		DLType g;
	public:
		VLSSS(LSSSPolicy,DLType g);
		int genVerifiableTokens(DLType**,size_t*);
		bool verify(DLType*,size_t,Share<SecretType>);
};

int calcW(LSSSPolicy policy,char** shareLabels,size_t shareCnt,int* w,int* t);

#endif