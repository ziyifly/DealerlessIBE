#include "ECDSA.h"

int ECDSAVerifyKey::toString(char*,size_t)
{
	return 0;
}

int ECDSAVerifyKey::toBinary(byte*,size_t)
{
	return 0;
}

ECDSAVerifyKey::ECDSAVerifyKey(ECDSACurve curve,ECn qA): curve(curve), qA(qA)
{
}

bool ECDSAVerifyKey::verify(BinaryData msg,ECDSASignature sig)
{
	Big z = HashToBig(msg);
	
#ifdef DEBUG
	showMsg(z,"z=");
#endif

	Big w = inverse(sig.s,curve.n);
	Big u1 = z*w % curve.n;
	Big u2 = sig.r*w % curve.n;
	Big x;
	mul(u1,curve.g,u2,qA).get(x);
	
#ifdef DEBUG
	showMsg(x,"x=");
#endif
	
	return (sig.r == x%curve.n);
}