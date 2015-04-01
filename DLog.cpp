#include "LSSS.h"

IntGroup& IntGroup::operator*=(IntGroup a)
{
	this->num = (this->num * a.num) % this->n;
	return *this;
}	
IntGroup operator*(IntGroup a,IntGroup b)
{
	IntGroup c(a.num,a.n);
	return c*=b;
}
IntGroup _pow(IntGroup a,int power)
{
	IntGroup b(1,a.n);
	for(unsigned int i=1<<31;i>0;i>>=1)
	{
		b *= b;
		if(power & i)
			b *= a;
	}
	return b;
}
bool operator==(IntGroup a,IntGroup b)
{
	return a.num==b.num && a.n==b.n;
}
int IntGroup::toString(char*,size_t)
{
	return 0;
}
int IntGroup::toBinary(byte*,size_t){}
//------------------------------------------------
BigGroup& BigGroup::operator*=(BigGroup a)
{
	this->num = (this->num * a.num) % this->n;
	return *this;
}
BigGroup operator*(BigGroup a,BigGroup b)
{
	BigGroup c(a.num,a.n);
	return  c*=b;
}
BigGroup _pow(BigGroup a,int power)
{
	return BigGroup(pow(a.num,power,a.n),a.n);
}
BigGroup _pow(BigGroup a,Big power)
{
	return BigGroup(pow(a.num,power,a.n),a.n);
}
bool operator==(BigGroup a,BigGroup b)
{
	return a.num==b.num && a.n==b.n;
}
int BigGroup::toString(char*,size_t)
{
	return 0;
}
int BigGroup::toBinary(byte*,size_t){}
//------------------------------------------------
ECGroup& ECGroup::operator*=(ECGroup a)
{
	this->p += a.p;
	return *this;
}
ECGroup operator*(ECGroup a,ECGroup b)
{
	ECGroup c(a.p);
	c.p += b.p;
	return c;
}
ECGroup _pow(ECGroup a,int power)
{
	return ECGroup(power*a.p);
}
ECGroup _pow(ECGroup a,Big power)
{
	return ECGroup(power*(a.p));
}
bool operator==(ECGroup a,ECGroup b)
{
	return a.p==b.p;
}
int ECGroup::toString(char* buf,size_t sz)
{
	//cout<<"in toString"<<endl;
	Big x,y;
	this->p.get(x,y);
	//cout<<"--- "<<x<<' '<<y<<endl;
	char* ptr = buf;
	*(ptr++) = '(';
	ptr<<x;
	ptr+=strlen(ptr);
	*(ptr++) = ',';
	ptr<<y;
	ptr+=strlen(ptr);
	*(ptr++) = ')';
	*(ptr++) = '\0';
	return 0;
}
int ECGroup::toBinary(byte*,size_t)
{
}