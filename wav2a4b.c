/*--------------------------------------------------------------------*/
/* wav2a4b - GienekP                                                  */
/*--------------------------------------------------------------------*/
#include <stdio.h>
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
unsigned int value(U8 *p)
{
	unsigned int ret,b;
	b=p[0];
	ret=b;
	b=p[1];
	ret|=(b<<8);
	b=p[2];
	ret|=(b<<16);
	b=p[3];
	ret|=(b<<24);	
	return ret;
}
/*--------------------------------------------------------------------*/
void convert(U8 *in, U8 *out, U8 *conv)
{
	unsigned int u,e;
	int i;
	double x;
	U8 b;
	u=in[1];
	u<<=8;
	u|=in[0];
	u<<=16;
	i=u;
	i/=0x10000;	
	x=(double)(i);	
	x+=32768.0;
	x/=65536.0;
	x*=15.5;
	e=x;
	b=e;
	e<<=3;
	*conv=b;
	out[0]=0;
	out[1]=e;
}
/*--------------------------------------------------------------------*/
void wav24b(const char *win, const char *bout, const char *wout)
{
	U8 hdr[44];
	unsigned int i;
	U8 h=0,c;
	FILE *pwin, *pbout, *pwout;
	pwin=fopen(win,"rb");
	if (pwin)
	{
		printf("Open \"%s\"\n",win);
		i=fread(hdr,sizeof(U8),sizeof(hdr),pwin);
		if (i==sizeof(hdr))
		{
			if ((hdr[0]=='R') && (hdr[1]=='I') && (hdr[2]=='F') && (hdr[3]=='F') && 
			    (hdr[8]=='W') && (hdr[9]=='A') && (hdr[10]=='V') && (hdr[11]=='E') &&
			    (hdr[20]==1) && (hdr[21]==0) && (hdr[22]==1) && (hdr[23]==0) && (value(&hdr[16])==0x00000010))
			{
				unsigned int filesize=(value(&hdr[4])+8);
				unsigned int size=(value(&hdr[40])/2);
				printf("FileSize=%i\n",filesize);
				printf("Size=%i\n",size);
				pbout=fopen(bout,"wb");
				if (pbout)
				{
					printf("Open \"%s\"\n",bout);
					pwout=fopen(wout,"wb");
					if (pwout)
					{
						printf("Open \"%s\"\n",wout);
						fwrite(hdr,sizeof(U8),sizeof(hdr),pwout);
						for (i=0; i<size; i++)
						{
							U8 in[2],out[2],conv[1];
							fread(in,sizeof(U8),sizeof(in),pwin);
							convert(in,out,conv);
							fwrite(out,sizeof(U8),sizeof(out),pwout);
							if (h==0) {c=conv[0];h=1;}
							else
							{
								c&=0x0F;
								c<<=4;
								conv[0]|=c;
								fwrite(conv,sizeof(U8),sizeof(conv),pbout);
								h=0;
							};
						};
						for (i=0; i<(filesize-44-size*2); i++)
						{
							U8 buf[1];
							fread(buf,sizeof(U8),sizeof(buf),pwin);
							fwrite(buf,sizeof(U8),sizeof(buf),pwout);
						};
						fclose(pwout);
					};
					fclose(pbout);
				};
			}
			else
			{
				printf("WAV file unsupported.\n");
			};
		};
		fclose(pwin);
	};
}
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	printf("wav2a4b (c)GienekP\n");
	if (argc==4)
	{
		wav24b(argv[1],argv[2],argv[3]);
	}
	else
	{
		printf("use:\n");
		printf("   wav2a4b filein.wav fileout.bin test.wav\n");
	};
	return 0;
}
/*--------------------------------------------------------------------*/
