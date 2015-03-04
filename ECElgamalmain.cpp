#include <cstdio>
#include "ECElgamal.h"
#include <cstring>
#include <fstream>
using namespace std;

#define COMMON "../lib/miracl/common.ecs"
#define PUBKEY "ecsPubKey"
#define PRIKEY "ecsPriKey"

#define DEBUG


Miracl precision = 20;

int main()
{	
	char buf[500];

	ifstream common(COMMON);	/* construct file I/O streams */
	//ofstream public_key(PUBKEY);
	//ofstream private_key(PRIKEY);
	int bits,ep;
	miracl *mip=&precision;

	ECn G,W;
	Big a,b,p,q,x,y,d;
	long seed = 123456789;

	//cout << "Enter 9 digit random number seed  = ";
	//cin >> seed;
	irand(seed);

	common >> bits;
	mip->IOBASE=16;
	common >> p >> a >> b >> q >> x >> y;
	mip->IOBASE=10;

	ecurve(a,b,p,MR_PROJECTIVE);

	cout<<"Curve input"<<endl;
	
	if (!G.set(x,y))
	{
		cout << "Problem - point (x,y) is not on the curve" << endl;
		return 0;
	}
	W=G;
	W*=q;
	if (!W.iszero())
	{
		cout << "Problem - point (x,y) is not of order q" << endl;
		return 0;
	}

	ECDSACurve curve = {G,q};
	cout<<"Curve set"<<endl;
	cout<<G<<endl;
	cout<<q<<endl;
	
	ECElgamalDecryptKey dk(curve);
	cout<<"DecryKey Generated"<<endl;
	
	ECElgamalEncryptKey ek = dk.getEncrpytKey();
	cout<<"EncryptKey Generated"<<endl;
	
	Big r = rand(curve.n-1);
	ECn msg = r * curve.g;
	
	ECElgamalCiphertext ct = ek.encrypt(msg);
	cout<<"Msg encrpted."<<endl;
	ct.toString(buf,500);
	cout<<"CT = "<<ct<<endl;
	
	ECn msgPrime = dk.decrypt(ct);
	cout<<"Msg' ="<<msgPrime<<endl;
	cout<<"Msg  ="<<msg<<endl;
	
	return 0;
}