#ifndef DLOG_H
#define DLOG_H

#include "pairing_3.h"

struct IntGroup
{
	unsigned int num,n;
	IntGroup(unsigned int num,unsigned int n): num(num),n(n) {};
	IntGroup() {};
	
	IntGroup& operator*=(IntGroup);
	
	friend IntGroup operator*(IntGroup,IntGroup);
	friend IntGroup _pow(IntGroup,int);
	friend bool operator==(IntGroup,IntGroup);
};

struct BigGroup
{
	Big num,n;
	
	BigGroup(const Big& num,const Big& n): num(num),n(n) {};
	BigGroup() {};
	
	BigGroup& operator*=(BigGroup);
	
	friend BigGroup operator*(BigGroup,BigGroup);
	friend BigGroup _pow(BigGroup,int);
	friend BigGroup _pow(BigGroup,Big);
	friend bool operator==(BigGroup,BigGroup);
};

struct ECGroup
{
	ECn p;
	
	ECGroup(const ECn p): p(p) {};
	ECGroup() {};
	
	ECGroup& operator*=(ECGroup);
	//ECGroup& operator=(ECGroup a) {p=a.p;}
	
	friend ECGroup operator*(ECGroup,ECGroup);
	friend ECGroup _pow(ECGroup,int);
	friend ECGroup _pow(ECGroup,Big);
	friend bool operator==(ECGroup,ECGroup);
};

extern PFC* _pfc;
void DLogSetPFC(PFC* pfc);

struct G1Group
{
	G1 g;
		
	G1Group(const G1& g): g(g) {};
	G1Group() {};
	
	G1Group& operator*=(G1Group a) {g = g + a.g; return *this;}
	G1Group& operator=(G1Group a) {g = a.g; return *this;}
	
	friend G1Group operator*(G1Group a,G1Group b) {return G1Group(a.g+b.g);}
	friend G1Group _pow(G1Group a,int p) {return G1Group(_pfc->mult(a.g,p));}
	friend G1Group _pow(G1Group a,Big p) {return G1Group(_pfc->mult(a.g,p));}
	friend bool operator==(G1Group a,G1Group b) {return a.g == b.g;}
};

struct G2Group
{
	G2 g;
		
	G2Group(const G2& g): g(g) {};
	G2Group() {};
	
	G2Group& operator*=(G2Group a) {g = g + a.g; return *this;}
	G2Group& operator=(G2Group a) {g = a.g; return *this;}
	
	friend G2Group operator*(G2Group a,G2Group b) {return G2Group(a.g+b.g);}
	friend G2Group _pow(G2Group a,int p) {return G2Group(_pfc->mult(a.g,p));}
	friend G2Group _pow(G2Group a,Big p) {return G2Group(_pfc->mult(a.g,p));}
	friend bool operator==(G2Group a,G2Group b) {return a.g == b.g;}
};

struct GTGroup
{
	GT g;
		
	GTGroup(const GT& g): g(g) {};
	GTGroup() {};
	
	GTGroup& operator*=(GTGroup a) {g = g * a.g; return *this;}
	GTGroup& operator=(GTGroup a) {g = a.g; return *this;}
	
	friend GTGroup operator*(GTGroup a,GTGroup b) {return GTGroup(a.g*b.g);}
	friend GTGroup _pow(GTGroup a,int p) {return GTGroup(_pfc->power(a.g,p));}
	friend GTGroup _pow(GTGroup a,Big p) {return GTGroup(_pfc->power(a.g,p));}
	friend bool operator==(GTGroup a,GTGroup b) {return a.g == b.g;}
};
#endif