#include "ECDSASignKey_err.h"
#include "Crypto119.h"
#include "ecn.h"
#include "big.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
void showMsg(Big n,const char* msg);
#endif

Big HashToBig(BinaryData data);

struct ECDSACurve
{
	ECn g;
	Big n;
};

class ECDSASignature: public Serialization
{
	private:
		Big r;
		Big s;
		ECDSASignature();
	
	public:
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		ECDSASignature(char*,size_t);
		
	friend class ECDSAVerifyKey;
	friend class ECDSASignKey;
};

class ECDSAVerifyKey: public VerifyKey<BinaryData,ECDSASignature>
{
	private:
		ECDSACurve curve;
		ECn qA;
	public:
		ECDSAVerifyKey(char*,size_t);
		ECDSAVerifyKey(byte*,size_t);
		ECDSAVerifyKey(ECDSACurve,ECn);
	
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		bool verify(BinaryData,ECDSASignature);
	friend class ECDSASignKey;
};

class ECDSASignKey: public SignKey<BinaryData,ECDSASignature,ECDSAVerifyKey>
{
	private:
		ECDSACurve curve; 
		Big dA;
	public:
		ECDSASignKey(Big dA,ECn g,Big n);
		ECDSASignKey(ECn g,Big n);
		ECDSASignKey(ECDSACurve curve);
		
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		ECDSAVerifyKey getVerifyKey();
		ECDSASignature sign(BinaryData,int* err=0);
};
