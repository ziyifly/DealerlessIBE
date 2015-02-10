#include "ECDSA.h"

#ifdef DEBUG
void showMsg(Big n,const char* msg)
{
	cout<<"log:"<<msg<<' '<<n<<endl;
}
#endif

Big HashToBig(BinaryData data)
{
	char s[32];
	Big z;
	sha256 sh;
	shs256_init(&sh);
	while(data.sz--)
	{
		shs256_process(&sh,*(data.data++));
	}
	shs256_hash(&sh,s);
	z = from_binary(32,s);
	return z;
}