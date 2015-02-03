// Non-implemant classes

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

template<class idType,class IBESecretKeyType>
class IBEMasterKey: public Serialization
{
	virtual int genSecretKey(idType);
};
