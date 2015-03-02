#include <cstdio>
#include "ECDSA.h"
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
	/* generate public/private keys */
	/*
	d=rand(q);
	//for (int i=0;i<=10000;i++)
	G*=d;
	ep=G.get(x);
	cout << "public key = " << ep << " " << x << endl;
	public_key << ep << " " << x << endl;
	private_key << d << endl;
	return 0;
	*/
	
	ECDSASignKey sk(curve);
	cout<<"SignKey generated"<<endl;
	
	ECDSAVerifyKey vk = sk.getVerifyKey();
	cout<<"VerifiyKey generated"<<endl;
	
	BinaryData msg;
	msg.data = (byte*)malloc(sizeof(byte)*6);
	msg.sz = 6;
	memcpy(msg.data,"hello",6);
	
	ECDSASignature sig = sk.sign(msg);
	sig.toString(buf,500);
	
	cout<<"Sign finished"<<endl;
	cout<<buf<<endl;
	
	//cout<<sig.r<<endl;
	//cout<<sig.s<<endl;
	ECDSASignature sigPrime(buf,500);
	
	bool result = vk.verify(msg,sigPrime);
	cout<<(result?"Verified":"Not Verified")<<endl;
	
	sk.toString(buf,500);
	cout<<"SK = "<<buf<<endl;
	vk.toString(buf,500);
	cout<<"VK = "<<buf<<endl;
	
	return 0;
}