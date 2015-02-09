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
		virtual SigType sign(MsgType) = 0;
};

template<class PlaintextType,class CiphertextType>
class EncryptKey: public Serialization
{
	public:
		virtual int encrypt(PlaintextType,CiphertextType);
};

template<class PlaintextType,class CiphertextType>
class DecryptKey: public Serialization
{
	public:
		virtual int decrypt(PlaintextType,CiphertextType);
		virtual EncryptKey<PlaintextType,CiphertextType> getEncrpytKey();
};

class Ciphertext: public Serialization
{
};

template<class idType,class IBESecretKeyType>
class IBEMasterKey: public Serialization
{
	virtual int genSecretKey(idType);
};
