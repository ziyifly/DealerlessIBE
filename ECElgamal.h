#include "Crypto119.h"
#include "ecn.h"
#include "big.h"

struct ECElgamalCurve
{
	ECn g;
	Big n;
};

class ECElgamalCiphertext: public Serialization
{
	private:
		ECn c1,c2;
		ECElgamalCiphertext();
	public:
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		ECElgamalCiphertext(char*,size_t);
		
	friend class ECElgamalEncryptKey;
	friend class ECElgamalDecryptKey;
};

class ECElgamalEncryptKey: public EncryptKey<ECn,ECElgamalCiphertext>
{
	private:
		ECElgamalCurve curve;
		ECn h;
	public:
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		ECElgamalEncryptKey(char*,size_t);
		ECElgamalEncryptKey(byte*,size_t);
		ECElgamalEncryptKey(ECElgamalCurve,ECn);
		
		ECElgamalCiphertext encrypt(ECn);
};

class ECElgamalDecryptKey: public DecryptKey<ECn,ECElgamalCiphertext,ECElgamalEncryptKey>
{
	private:
		ECElgamalCurve curve;
		Big x;
	public:
		int toString(char*,size_t);
		int toBinary(byte*,size_t);
		
		ECElgamalDecryptKey(char*,size_t);
		ECElgamalDecryptKey(byte*,size_t);
		ECElgamalDecryptKey(Big x,ECn g,Big n);
		ECElgamalDecryptKey(ECn g,Big n);
		ECElgamalDecryptKey(ECElgamalCurve curve);
	
		ECn decrypt(ECElgamalCiphertext);
		ECElgamalEncryptKey getEncrpytKey();
};