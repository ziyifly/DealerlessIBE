#include <cstdlib>
//#include "miracl.h"

typedef unsigned char byte;

class Serialization
{
	public:
		virtual int toString(char*,size_t) = 0;
		virtual int toBinary(byte*,size_t) = 0;
};
