#include "pairing_3.h"

#include "ECDSA.h"
#include "ECElgamal.h"
#include "LSSS.h"

#include "miracl.h"

int loadCurve(const char* curveFile,ECn &G,Big &q,miracl* precision);
ECDSACurve loadECDSACurve(const char* curveFile,miracl* precision);
ECElgamalCurve loadECElgamalCurve(const char* curveFile,miracl* precision);

LSSSPolicy loadPolicy(const char* policyFile);
void clearPolicy(LSSSPolicy policy);

void getFileName(char* fileName,const char* name,const char* ext);
void getPath(char* path,const char* dirName,const char* fileName);

size_t inputFromFile(char* str,const char* fileName);
void outputToFile(const char* str,const char* fileName);

size_t inputBinaryFromFile(BinaryData* buf,const char* fileName);
void outputBinaryToFile(BinaryData buf,const char* fileName);

ECn ECnFromStr(char* buf);
ECn2 ECn2FromStr(char* buf);
ZZn12 ZZn12FromStr(char* buf);
G1 G1FromFile(const char* filePath);
G2 G2FromFile(const char* filePath);
GT GTFromFile(const char* filePath);
Big BigFromFile(const char* filePath);

void hashBig(Big x,char symKeyBuf[20]);
void aesEncrypt(char key[],char text[],size_t sz,char iv[]);
void aesDecrypt(char key[],char text[],size_t sz,char iv[]);