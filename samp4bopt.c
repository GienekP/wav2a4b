/*--------------------------------------------------------------------*/
/* samp4bopt - GienekP                                                */
/*--------------------------------------------------------------------*/
#include <stdio.h>
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
unsigned int optimize(U8 *dta, unsigned int size)
{
	unsigned int i,ret=0,start=0,stop=size;
	for (i=(size-1); i>2; i--)
	{
		if (dta[i-1]==0x77) {stop--;} else {i=1;};
	};
	for (i=1; i<stop; i++)
	{
		if (dta[i]==0x77) {start++;} else {i=stop;};
	};
	ret=(stop-start);
	for (i=0; i<ret; i++)
	{
		dta[i]=dta[i+start];
	};
	return ret;
}
/*--------------------------------------------------------------------*/
void samp4bopt(const char *fin, const char *fout)
{
	U8 buf[65536];
	const unsigned int sb=sizeof(buf);
	unsigned int i,size,opt;
	FILE *pin, *pout;
	for (i=0; i<sb; i++) {buf[i]=0;};
	pin=fopen(fin,"rb");	
	if (pin)
	{
		size=fread(buf,sizeof(U8),sb,pin);
		opt=size;
		printf("Read \"%s\" [%i bytes]\n",fin,size);
		fclose(pin);
		if (size) {opt=optimize(buf,size);};
		pout=fopen(fout,"wb");
		if (pout)
		{
			i=fwrite(buf,sizeof(U8),opt,pout);	
			printf("Save \"%s\" [%i bytes]\n",fout,i);
			fclose(pout);
		};
		printf("Optimized %i bytes\n",size-opt);
	};
}
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	printf("samp4bopt (c)GienekP\n");
	if (argc==3)
	{
		samp4bopt(argv[1],argv[2]);
	}
	else
	{
		printf("use:\n");
		printf("   samp4bopt input.bin output.bin\n");
	};
	return 0;
}
/*--------------------------------------------------------------------*/
