#include "Serialization.h"

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

template<class SecretType,class ShareType>
class LSSS
{
	private:
		bool policySet,secretSet;
		LSSSPolicy policy;
		SecretType* r;
	public:
		LSSS();
		int setPolicy(LSSSPolicy);
		int setSecretVector(SecretType*);
		int genShares(ShareType**,size_t*);
		int reconstructSecret(ShareType* shares,size_t,SecretType** secret);
};
