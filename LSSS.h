#include "Crypto119.h"
#include "big.h"
#include "ecn.h"
#include <cstring>
#include <cstdlib>

struct IntGroup: public Serialization
{
	unsigned int num,n;
	IntGroup(unsigned int num,unsigned int n): num(num),n(n) {};
	IntGroup() {};
	
	IntGroup& operator*=(IntGroup);
	
	int toString(char*,size_t);
	int toBinary(byte*,size_t);
	
	friend IntGroup operator*(IntGroup,IntGroup);
	friend IntGroup _pow(IntGroup,int);
	friend bool operator==(IntGroup,IntGroup);
};

struct BigGroup: public Serialization
{
	Big num,n;
	
	BigGroup(Big num,Big n): num(num),n(n) {};
	BigGroup() {};
	
	BigGroup& operator*=(BigGroup);
	
	int toString(char*,size_t);
	int toBinary(byte*,size_t);
	
	friend BigGroup operator*(BigGroup,BigGroup);
	friend BigGroup _pow(BigGroup,int);
	friend BigGroup _pow(BigGroup,Big);
	friend bool operator==(BigGroup,BigGroup);
};

struct ECGroup: public Serialization
{
	ECn p;
	
	ECGroup(ECn p): p(p) {};
	ECGroup() {};
	
	ECGroup& operator*=(ECGroup);
	//ECGroup& operator=(ECGroup a) {p=a.p;}
	
	int toString(char*,size_t);
	int toBinary(byte*,size_t);
	
	friend ECGroup operator*(ECGroup,ECGroup);
	friend ECGroup _pow(ECGroup,int);
	friend ECGroup _pow(ECGroup,Big);
	friend bool operator==(ECGroup,ECGroup);
};

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
	protected:
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
		VLSSS(LSSSPolicy,DLType g);
		int getVerifiableTokens(DLType**,size_t*);
		bool verify(DLType*,size_t,Share<SecretType>);
};