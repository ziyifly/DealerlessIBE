#include "Serialization.h"
// Non-implemant classes

template<class MsgType,class SigType>
class SignKey:public Serialization
{
	public:
		virtual VerifyKeyType getVerifyKey() = 0;
		virtual SigType sign(MsgType) = 0;
};

template<class MsgType,class SigType>
class VerifyKey: public Serialization
{
	public:
		virtual bool verify(MsgType,SigType) = 0;
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

template<class idType,class IBESecretKeyType>
class IBEMasterKey: public Serialization
{
	virtual int genSecretKey(idType);
};
