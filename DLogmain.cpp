#include "LSSS.h"
#include <cstdio>
#include <iostream>
using namespace std;

Miracl precision = 20;

int main()
{
	IntGroup a(2,10),b(3,10);
	
	printf("a = %u\n",a.num);
	printf("b = %u\n",b.num);
	printf("a*b = %u\n",(a*b).num);
	
	printf("a^10 = %u\n",_pow(a,10).num);
	printf("b^10 = %u\n",_pow(b,10).num);
	
	Big p(10);
	BigGroup c(2,10);
	BigGroup d(3,10);
	cout<<"c = "<<c.num<<endl;
	cout<<"d = "<<d.num<<endl;
	cout<<"c*d = "<<(c*d).num<<endl;
	
	cout<<"c^10 = "<<_pow(c,10).num<<endl;
	cout<<"d^10 = "<<_pow(d,10).num<<endl;
	cout<<"d^p = "<<_pow(d,p).num<<endl;
	cout<<"--- end ---"<<endl;
	return 0;
}