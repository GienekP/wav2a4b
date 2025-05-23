/*--------------------------------------------------------------------*/
/* wav2a4b                                                            */
/* by GienekP                                                         */
/* (c) 2025                                                           */
/*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
const unsigned int FREQTARGET=3906;
const unsigned int LENGTHALIGN=256;
/*--------------------------------------------------------------------*/
/* Input WAV */
double *samples;
unsigned int musicsize;
unsigned int nochannels;
unsigned int samplerate;
/* Result */
double *resamples;
U8 *data;
unsigned int datasize;
/*--------------------------------------------------------------------*/
void resample_fft(double *in, unsigned int insize, unsigned int infreq,
				double *out, unsigned int outsize, unsigned int outfreq)
{
	unsigned int i,j,limit;
	const double pi=2.0*acos(0.0);
	const double N=(double)(insize);
	double x,y,am,ph,freq,t,w,xn;
	printf("FFT Resample from %iHz to %iHz\n",infreq,outfreq);
	for (i=0; i<outsize; i++) {out[i]=0.0;};	// clear output
	limit=outfreq*insize/(2*infreq);	// limit half outfreq
	for (i=1; i<limit; i++)	// 1=> without offset; only pos freq
	{
		freq=((double)(i)*(double)(infreq)/(double)(insize));
		printf("%i%%     \n\033[A",100*i/limit);
		if (freq<((double)(outfreq)/2.0)) // Protect Nyquist limit
		{
			x=0;
			y=0;
			for (j=0; j<insize; j++)
			{
				w=-2.0*pi*(double)(i*j)/N;
				xn=in[j];
				x=x+xn*cos(w);
				y=y+xn*sin(w);	
			};
			am=sqrt(x*x+y*y);
			ph=atan2(y,x);
			for (j=0; j<outsize; j++)
			{
				t=(double)(j)/(double)(outfreq);
				out[j]+=(am*sin(2.0*pi*freq*t+ph));
			};
		}
		else {i=limit;};
	};	
}
/*--------------------------------------------------------------------*/
void resample()
{
	/* OLD AVERAGE METHOD - FASTES
	unsigned int i,p1,p2;
	double p,s,s1,s2;
	for (i=0; i<datasize; i++)
	{
		p=(double)(i)*musicsize/datasize;
		p1=(unsigned int)(p);
		p2=(p1+1);
		s1=samples[p1];
		s2=samples[p2];
		p=(p-(double)(p1));
		s=(s2*p+s1*(1.0-p));
		resamples[i]=s;
	};*/
	resample_fft(samples,musicsize,samplerate,resamples,datasize,FREQTARGET);
}
/*--------------------------------------------------------------------*/
void killsilent(void)
{
	unsigned int i,start=0,stop=datasize;
	U8 last;
	last=data[datasize-1];
	if ((last==0x07) || (last=0x08))
	{
		printf("Kill silent $%02x\n",last);
		for (i=1; i<(datasize-2); i++)
		{
			if ((data[i])!=last) {start=i;i=datasize;};
		};
		printf("Start sample %i\n",start);
		for (i=(datasize-1); i>2; i--)
		{
			if ((data[i])!=last) {stop=i;i=1;};
		};
		printf("Stop sample %i\n",stop);
		for (i=start; i<stop; i++) {data[i-start]=data[i];};
		datasize=stop;
		datasize-=start;
		printf("New size %i\n",datasize);
	};
}
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
void convert()
{
	double x,min=0,max=0;
	unsigned int i;
	for (i=0; i<datasize; i++)
	{
		x=resamples[i];
		if (x<min) {min=x;};
		if (x>max) {max=x;};
	};
	printf("min=%0.0f\nmax=%0.0f\n",min,max);
	for (i=0; i<datasize; i++)
	{
		x=resamples[i];
		x+=(-1.0*min);
		x/=(max-min);
		x*=15.5;
		data[i]=(((U8)(x))&0x0F);
	};	
}
/*--------------------------------------------------------------------*/
void saveXEX(const char *xexfn)
{
	const U8 xex[173]={
		  0xff, 0xff, 0xe0, 0x02, 0xe1, 0x02, 0x00, 0x20, 0x00, 0x20, 0x9e, 0x20, 0xa9, 0x00, 0x8d, 0x1a,
		  0xd0, 0x8d, 0x00, 0xd4, 0x8d, 0x2f, 0x02, 0x8d, 0x00, 0xd2, 0x8d, 0x02, 0xd2, 0x8d, 0x04, 0xd2,
		  0x8d, 0x06, 0xd2, 0x8d, 0x01, 0xd2, 0x8d, 0x03, 0xd2, 0x8d, 0x05, 0xd2, 0x8d, 0x07, 0xd2, 0x8d,
		  0x08, 0xd2, 0xa9, 0x17, 0x8d, 0x07, 0xd2, 0x78, 0xa9, 0x02, 0x85, 0xa0, 0xa9, 0x21, 0x85, 0xa1,
		  0xa0, 0x00, 0x20, 0x8b, 0x20, 0xb1, 0xa0, 0xaa, 0x29, 0x0f, 0x09, 0x10, 0x85, 0xa3, 0x8d, 0x0a,
		  0xd4, 0x8a, 0x4a, 0x4a, 0x4a, 0x4a, 0x09, 0x10, 0x85, 0xa2, 0x8d, 0x0a, 0xd4, 0xe6, 0xa0, 0xd0,
		  0x02, 0xe6, 0xa1, 0xa5, 0xa0, 0xcd, 0x00, 0x21, 0xd0, 0x0a, 0xa5, 0xa1, 0xcd, 0x01, 0x21, 0xd0,
		  0x03, 0x4c, 0x2c, 0x20, 0x8d, 0x0a, 0xd4, 0xa5, 0xa2, 0x8d, 0x0a, 0xd4, 0x8d, 0x07, 0xd2, 0x8d,
		  0x0a, 0xd4, 0x8a, 0x8d, 0x0a, 0xd4, 0x8d, 0x1a, 0xd0, 0x8d, 0x0a, 0xd4, 0xa5, 0xa3, 0x8d, 0x0a,
		  0xd4, 0x8d, 0x07, 0xd2, 0x4c, 0x39, 0x20, 0xa2, 0x20, 0xa9, 0x70, 0xcd, 0x0b, 0xd4, 0xd0, 0xfb,
		  0xa9, 0x72, 0xcd, 0x0b, 0xd4, 0xd0, 0xfb, 0xca, 0xd0, 0xef, 0x60, 0x00, 0x21};
	U8 b[1];
	unsigned int i;
	FILE *pf;
	pf=fopen(xexfn,"wb");
	if (pf)
	{
		unsigned int ds=(datasize/2);
		if (ds>40000) {ds=40000;};
		fwrite(xex,sizeof(U8),sizeof(xex),pf);
		b[0]=((ds+1)&0xFF);
		fwrite(b,sizeof(U8),sizeof(b),pf);
		b[0]=(((ds+1)>>8)&0xFF)+0x21;
		fwrite(b,sizeof(U8),sizeof(b),pf);
		b[0]=((ds+2)&0xFF);
		fwrite(b,sizeof(U8),sizeof(b),pf);
		b[0]=(((ds+2)>>8)&0xFF)+0x21;
		fwrite(b,sizeof(U8),sizeof(b),pf);
		for (i=0; i<ds; i++)
		{
			b[0]=((data[i*2]&0x0F)<<4)+(data[i*2+1]&0x0F);
			fwrite(b,sizeof(U8),sizeof(b),pf);
		};				
		fclose(pf);
		printf("Save \"%s\"\n",xexfn);
	}
	else
	{
		printf("Can't save \"%s\"\n",xexfn);
	};
}
/*--------------------------------------------------------------------*/
void saveATARI(const char *atarifn)
{
	unsigned int i,j=0;
	U8 b[1];
	FILE *pf;
	pf=fopen(atarifn,"wb");
	if (pf)
	{
		for (i=0; i<(datasize/2); i++)
		{
			b[0]=((data[i*2]&0x0F)<<4)+(data[i*2+1]&0x0F);
			j+=1;
			fwrite(b,sizeof(U8),sizeof(b),pf);
		};
		b[0]=0x77;
		j=((((j/LENGTHALIGN)+1)*LENGTHALIGN)-j);
		for (i=0; i<j; i++) {fwrite(b,sizeof(U8),sizeof(b),pf);};
		printf("Save \"%s\"\n",atarifn);
	}
	else
	{
		printf("Can't save \"%s\"\n",atarifn);
	};
}
/*--------------------------------------------------------------------*/
void putInt(unsigned int val, U8 *pos)
{
	pos[0]=(val&0xFF);
	pos[1]=((val>>8)&0xFF);
	pos[2]=((val>>16)&0xFF);
	pos[3]=((val>>24)&0xFF);	
}
/*--------------------------------------------------------------------*/
void saveWAV(const char *testwavfn)
{
	U8 hdr[44]={0x52,0x49,0x46,0x46,0xFF,0xFF,0xFF,0xFF,0x57,0x41,0x56,0x45,0x66,0x6D,0x74,0x20,
				0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0xEE,0xEE,0xEE,0xEE,0xDD,0xDD,0xDD,0xDD,
				0x01,0x00,0x08,0x00,0x64,0x61,0x74,0x61,0xCC,0xCC,0xCC,0xCC};
	unsigned int i;
	FILE *pf;
	pf=fopen(testwavfn,"wb");
	if (pf)
	{
		putInt(44+datasize-8,&hdr[4]);
		putInt(FREQTARGET,&hdr[24]);
		putInt(FREQTARGET,&hdr[28]);
		putInt(datasize,&hdr[40]);
		fwrite(hdr,sizeof(U8),sizeof(hdr),pf);
		for (i=0; i<datasize; i++)
		{
			U8 buf[1];
			buf[0]=((data[i]&0x0F)*17);
			fwrite(buf,sizeof(U8),sizeof(buf),pf);
		};
		fclose(pf);
		printf("Save \"%s\"\n",testwavfn);
	}
	else {printf("Can't save \"%s\"\n",testwavfn);};
}
/*--------------------------------------------------------------------*/
double sample(const U8 *buf)
{
	double ret;
	unsigned int i;
	int j;
	i=buf[1];
	i<<=8;
	i|=buf[0];
	i<<=16;
	j=i;
	j/=65536;
	ret=(double)(j);
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int loadWAV(const char *wavefilename)
{
	unsigned int i,ret=0;
	FILE *pf;	
	pf=fopen(wavefilename,"rb");
	if (pf)
	{
		fseek(pf,44,SEEK_SET);
		for (i=0; i<musicsize; i++)
		{
			U8 buf[2];
			double left,right;
			fread(buf,sizeof(U8),sizeof(buf),pf);
			left=sample(buf);
			if (nochannels==2)
			{
				fread(buf,sizeof(U8),sizeof(buf),pf);
				right=sample(buf);
			}
			else {right=0;};
			samples[i]=(left+right);
		};
		ret=1;
	};
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int detectWAV(const char *wavefilename)
{
	unsigned int i,ret=0,test=0;
	samples=NULL;
	resamples=NULL;
	data=NULL;
	musicsize=0;
	datasize=0;
	nochannels=0;
	samplerate=0;
	U8 hdr[44];
	FILE *pf;
	pf=fopen(wavefilename,"rb");
	if (pf)
	{
		printf("Open \"%s\"\n",wavefilename);
		i=fread(hdr,sizeof(U8),sizeof(hdr),pf);
		fclose(pf);
		if (i==sizeof(hdr))
		{
			if (value(&hdr[0])==0x46464952) {printf("Format: RIFF\n");test++;};
			printf("File size %i bytes\n",value(&hdr[4])+8);
			if (value(&hdr[8])==0x45564157) {printf("Type: WAVE\n");test++;};
			if (value(&hdr[12])==0x20746D66) {printf("Format chunk: fmt\n");test++;};
			if (value(&hdr[16])==16)
			{
				if ((hdr[20]==1) && (hdr[21]==0)) {printf("Type format: PCM\n");test++;};
				if (((hdr[22]==1) || (hdr[22]==2)) && (hdr[23]==0)) {nochannels=hdr[22];printf("Channels: %i\n",nochannels);test++;};
				samplerate=value(&hdr[24]);
				printf("Sample rate: %iHz\n",samplerate);
				if ((hdr[34]==16) && (hdr[35]==0)) {printf("16bit per sample\n");test++;};
				if ((value(&hdr[36])==0x61746164) && (test==6))
				{
					musicsize=(value(&hdr[40])/nochannels)/2;
					printf("Samples per channel: %i\n",musicsize);
					printf("Time %f seconds\n", ((double)(musicsize))/((double)(samplerate)));
					test++;
				};
			};
			if (test==7)
			{
				datasize=(unsigned long)FREQTARGET*musicsize/samplerate;
				if ((datasize&1)==1) {datasize--;};
				printf("WAV correctly detected\n");
				samples=(double *)malloc(musicsize*sizeof(double));
				if (samples)
				{
					printf("Alocated %i bytes for WAV\n",(int)(musicsize*sizeof(double)));
					test++;
				};
				resamples=(double *)malloc(datasize*sizeof(double));
				if (resamples)
				{
					printf("Alocated %i bytes for RESAMPLE\n",(int)(datasize*sizeof(double)));
					for (i=0; i<datasize; i++) {resamples[i]=0;};
					test++;
				};				
				data=(U8 *)malloc(datasize*sizeof(U8));
				if (data)
				{
					printf("Alocated %i bytes for RESULT\n",(int)(datasize*sizeof(U8)));
					for (i=0; i<datasize; i++) {data[i]=0;};
					test++;
				};
			}
			else
			{
				printf("WAV unsuported\n");
			};
		};
	}
	else
	{
		printf("Error \"%s\"\n",wavefilename);
	};
	if (test==10) {ret=1;};
	return ret;
}
/*--------------------------------------------------------------------*/
void wav24b(const char *win, const char *bout, const char *wout, const char *xout)
{
	if (detectWAV(win))
	{
		if (loadWAV(win))
		{
			resample();
			convert();
			killsilent();
			saveATARI(bout);
			if (wout) {saveWAV(wout);};
			if (xout) {saveXEX(xout);};
		};
	};
	if (samples) {free(samples);};
	if (data) {free(data);};
}
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	printf("wav2a4b (c)GienekP\n");
	switch (argc)
	{
		case 3:	{wav24b(argv[1],argv[2],NULL,NULL);} break;
		case 4:	{wav24b(argv[1],argv[2],argv[3],NULL);} break;
		case 5:	{wav24b(argv[1],argv[2],argv[3],argv[4]);} break;
		default:
		{
			printf("use:\n");
			printf("   wav2a4b filein.wav fileout.bin\n");
			printf("   wav2a4b filein.wav fileout.bin test.wav\n");
			printf("   wav2a4b filein.wav fileout.bin test.wav test.xex\n");
		};
	};
	return 0;
}
/*--------------------------------------------------------------------*/
