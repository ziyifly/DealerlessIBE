#include <cstdlib>

typedef unsigned char byte;

class Serialization
{
	public:
		virtual int toString(char*,size_t);
		virtual int toBinary(byte*,size_t);
};

template<class MsgType,class SigType,class VerifyKeyType>
class SignKey:public Serialization
{
	public:
		virtual VerifyKeyType getVerifyKey();
		virtual SigType sign(MsgType);
};

template<class MsgType,class SigType,class SignKeyType>
class VerifyKey: public Serialization
{
	public:
		virtual bool verify(MsgType,SigType);
		
		friend SignKeyType;
};

template<class SignKeyType,class VerifyKeyType>
class Signature: public Serialization
{
	friend SignKeyType;
	friend VerifyKeyType;
};

template<class PlaintextType,class CiphertextType,class EncryptKeyType>
class DecryptKey: public Serialization
{
	public:
		virtual int decrypt(PlaintextType,CiphertextType);
		virtual EncryptKeyType getEncrpytKey();
};

template<class PlaintextType,class CiphertextType,class DecryptKeyType>
class EncryptKey: public Serialization
{
	public:
		virtual int encrypt(PlaintextType,CiphertextType);
	
	friend DecryptKeyType;
};

template<class DecryptKeyType,class EncryptKeyType>
class Ciphertext: public Serialization
{
	friend DecryptKeyType;
	friend EncryptKeyType;
};

struct LSSSPolicy
{
	int** A;
	int rowCnt;
	int colCnt;
	char** labels;
};

template<class SecretType,class ShareType>
class LSSS: public Serialization
{
	private:
		bool policySet,secretSet;
		LSSSPolicy policy;
		SecretType* r;
		int randomR();
	public:
		int setPolicy(LSSSPolicy*);
		int setSecret(SecretType);
		int genShares(ShareType**,size_t*);
		int reconstructSecret(ShareType**,size_t,SecretType*);
};

template<class SecretType,class LSSS>
struct Share: public Serialization
{
	private:
		char* labels;
		SecretType share;
		
	friend LSSS;
};

template<class idType,class IBESecretKeyType>
class IBEMasterKey: public Serialization
{
	virtual int genSecretKey(idType);
};
