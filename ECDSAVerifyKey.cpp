#include "ECDSA.h"

int ECDSAVerifyKey::toString(char*,size_t)
{
	return 0;
}

int ECDSAVerifyKey::toBinary(byte*,size_t)
{
	return 0;
}

ECDSAVerifyKey::ECDSAVerifyKey(ECDSACurve curve,ECn qA)
{
	this->curve = curve;
	this->qA = qA;
}

bool ECDSAVerifyKey::verify(BinaryData msg,ECDSASignature sig)
{
	Big z = HashToBig(msg);
	
	Big w = inverse(sig.s,curve.n);
	Big u1 = z*w % curve.n;
	Big u2 = sig.r*w % curve.n;
	Big x;
	((u1*curve.g)+=(u2*qA)).getx(x);
	
	return (sig.r == x);
}