#include "dealerlessIBE.h"
#include "miracl.h"
#include "openssl.h"
//#include "ecn.h"
#include "big.h"

struct ECDSASignature;
class ECDSASignKey: SignKey<Serialization,ECDSASignature>;
class ECDSAVerifyKey: VerifyKey<Serialization,ECDSASignature>;

struct ECDSACurve
{
	ECn g;
	Big n;
}

struct ECDSASignature
{
	Big r;
	Big s;
};

class ECDSASignKey: public SignKey<Serialization,ECDSASignature>
{
	private:
		ECDSACurve curve; 
		Big dA;
	public:
		ECDSASignKey(char*,size_t);
		ECDSASignKey(byte*,size_t);
		ECDSASignKey(Big dA,ECn g,Big n);
		ECDSASignKey(ECn g,Big n);
		
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		ECDSAVerifyKey getVerifyKey();
		ECDSASignature sign(Serialization*,int*);
}

class ECDSAVerifyKey: public VerifyKey<Serialization,ECDSASignature>
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
		bool verify(Serialization,ECDSASignature);
}