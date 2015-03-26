#include "LSSS.h"

IntGroup::IntGroup(unsigned int num,unsigned int n):num(num),n(n){};

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

BigGroup::BigGroup(Big num,Big n):num(num),n(n){};
	
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