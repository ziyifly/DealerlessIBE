#include "Crypto119.h"
#include "big.h"
#include <cstring>
#include <cstdlib>

struct LSSSPolicy: public Serialization
{
	int** A;
	int rowCnt;
	int colCnt;
	char** labels;
	
	int toString(char*,size_t);
	int toBinary(byte*,size_t);
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
	private:
		bool secretSet;
		LSSSPolicy policy;
		SecretType* r;
	public:
		//LSSS();
		LSSS(LSSSPolicy);
		//int setPolicy(LSSSPolicy);
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
		int getVerifiableTokens(DLType**,size_t*);
		bool verify(Share<SecretType>);
};