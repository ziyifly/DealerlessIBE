#include "ECDSA.h"

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