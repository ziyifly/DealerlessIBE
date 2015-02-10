#include "Serialization.h"
// Non-implemant classes

template<class MsgType,class SigType>
class VerifyKey: public Serialization
{
	public:
		virtual bool verify(MsgType,SigType) = 0;
};

template<class MsgType,class SigType,class VerifyKeyType>
class SignKey:public Serialization
{
	public:
		virtual VerifyKeyType getVerifyKey() = 0;
		virtual SigType sign(MsgType,int*) = 0;
};

template<class PlaintextType,class CiphertextType>
class EncryptKey: public Serialization
{
	public:
		virtual int encrypt(PlaintextType,CiphertextType) = 0;
};

template<class PlaintextType,class CiphertextType,class EncryptKeyType>
class DecryptKey: public Serialization
{
	public:
		virtual int decrypt(PlaintextType,CiphertextType) = 0;
		virtual EncryptKeyType getEncrpytKey() = 0;
};

template<class idType,class IBESecretKeyType>
class IBEMasterKey: public Serialization
{
	virtual int genSecretKey(idType);
};
