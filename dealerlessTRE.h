#include "ECDSA.h"
#include "ECElgamal.h"
#include "LSSS.h"

#include "miracl.h"

int loadCurve(const char* curveFile,ECn &G,Big &q,miracl* precision);
ECDSACurve loadECDSACurve(const char* curveFile,miracl* precision);
ECElgamalCurve loadECElgamalCurve(const char* curveFile,miracl* precision);

LSSSPolicy loadPolicy(const char* policyFile);
void clearPolicy(LSSSPolicy policy);

void getPath(char* path,const char* dirName,const char* fileName);

void outputToFile(const char* str,const char* fileName);
size_t inputFromFile(char* str,const char* fileName);

ECn ECnFromStr(char* buf);
Big BigFromStr(char* buf);

void hashBig(sha *s,Big x);