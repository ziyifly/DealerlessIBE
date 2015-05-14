#include "LSSS.h"
#include "VLSSS_err.h"

#include "DLog.h"

//#define DEBUG

#ifdef DEBUG
void memdump(void* ptr,size_t n)
{
	for(int i=0;i<n;i++)
	{
		printf(" %02x",*((unsigned char*)ptr + i));
	}
	printf("\n");
}
#endif

template<class SecretType,class DLType>
VLSSS<SecretType,DLType>::VLSSS(LSSSPolicy policy,DLType g):LSSS<SecretType>(policy),g(g) {}

template<class SecretType,class DLType>
int VLSSS<SecretType,DLType>::genVerifiableTokens(DLType** tokens,size_t* sz)
{
	int err = GETTOKBASE;
	if(!this->secretSet)
	{
		err = GETTOKR;
	}
	else
	{
		size_t _sz = this->policy.colCnt;
		DLType* _tokens = new DLType[_sz];
		
		//cout<<_sz * sizeof(DLType)<<" bytes"<<endl;
		//cout<<"size = "<<_sz<<endl;
		for(int i=0;i<_sz;i++)
		{
			char buf[100];
			//cout<<this->r[i]<<endl;
			_tokens[i] = _pow(g,this->r[i]);
			//cout<<_tokens+i<<endl;
			//memdump(&_tokens[i],24);
			
			//cout<<"check 2"<<endl;
			//DLType tmp;
			//memdump(&tmp,24);
			//tmp = _pow(g,this->r[i]);
			//memdump(&tmp,24);
			//tmp.toString(buf,100);
			//_pow(g,this->r[i]).toString(buf,100);
			//cout<<&tmp<<endl;
			//cout<<buf<<endl;
			//_tokens[i] = _pow(g,this->r[i]);
			
			//cout<<"check 3"<<endl;
			//_tokens[i].toString(buf,100);    // <======= This Line?
			//cout<<buf<<endl;
		}
		//cout<<"check 4"<<endl;
		*tokens = _tokens;
		*sz = _sz;
	}
	return err;
}

template<class SecretType,class DLType>
bool VLSSS<SecretType,DLType>::verify(DLType* tokens,size_t tokenCnt,Share<SecretType> share)
{
	int rowNum = -1;
	for(int i=0;i<this->policy.rowCnt;i++)
	{
		if(strcmp(this->policy.labels[i],share.label)==0)
		{
			rowNum = i;
			break;
		}
	}

#ifdef DEBUG
	for(int i=0;i<this->policy.colCnt;i++)
	{
		cout<<tokens[i].g.g<<endl;
		cout<<this->policy.A[rowNum][i]<<endl;
	}
#endif
	
	DLType innerProduct;
	innerProduct = _pow(tokens[0],this->policy.A[rowNum][0]);
	for(int i=1;i<this->policy.colCnt;i++)
	{
		innerProduct = (innerProduct*_pow(tokens[i],this->policy.A[rowNum][i]));
	}

	return innerProduct == _pow(g,share.share);
}

//template class VLSSS<int,IntGroup>;
//template class VLSSS<Big,BigGroup>;
//template class VLSSS<Big,ECGroup>;

template class VLSSS<Big,G1Group>;
template class VLSSS<Big,G2Group>;
template class VLSSS<Big,GTGroup>;