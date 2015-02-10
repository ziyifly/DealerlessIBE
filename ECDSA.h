#include "ECDSASignKey_err.h"
#include "dealerlessIBE.h"
#include "ecn.h"
#include "big.h"

#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
void showMsg(Big n,const char* msg);
#endif

struct BinaryData
{
	byte* data;
	size_t sz;
};

Big HashToBig(BinaryData data);

struct ECDSACurve
{
	ECn g;
	Big n;
};

struct ECDSASignature
{
	Big r;
	Big s;
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
