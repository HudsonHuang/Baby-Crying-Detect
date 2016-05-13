#include "math.h"
#include"stdio.h"
#include"stdlib.h"
//精度0.0001弧度  
typedef struct complex //复数类型  
{  
  float real;       //实部  
  float imag;       //虚部  
}complex;  
  
#define PI 3.1415926535897932384626433832795028841971  
#define size 1411200
#define fs_div 5
#define path   2
#define  VN 131072
typedef  unsigned int uint32;
typedef  unsigned short ushort;



void conjugate_complex(int n,complex in[],complex out[])  
{  
  int i = 0;  
  for(i=0;i<n;i++)  
  {  
    out[i].imag = -in[i].imag;  
    out[i].real = in[i].real;  
  }   
}  
void c_pow(complex f[],float out[],int n)  
{  
  int i = 0;   
  for(i=0;i<n;i++)  
  {  
     out[i] = f[i].real * f[i].real + f[i].imag * f[i].imag;   
  }   
}  
void c_plus(complex a,complex b,complex *c)
{
	c->real=a.real + b.real;
	c->imag=a.imag + b.imag;
}
void c_sub(complex a,complex b,complex *c)
{
	c->real=a.real - b.real;
	c->imag=a.imag - b.imag;
}
void c_mul(complex a,complex b,complex *c)
{
	c->real= a.real * b.real - a.imag * b.imag;  
    c->imag = a.real * b.imag + a.imag * b.real;
}
void Wn_i(int n,int i,complex *Wn,char flag)  
{  
  Wn->real =cos(2*PI*i/n);  
  if(flag == 1)  
  Wn->imag = -sin(2*PI*i/n);  
  else  
  Wn->imag = sin(2*PI*i/n);  
}  
//傅里叶变化  
void fft(int N,complex f[])  
{  
  complex t,wn;//中间变量  
  int i,j,k,m,n,l,r,M;  
  int la,lb,lc;
  /*----计算分解的级数M=log2(N)----*/  
  for(i=N,M=1;(i=i/2)!=1;M++);   
  /*----按照倒位序重新排列原信号----*/  
  for(i=1,j=N/2;i<=N-2;i++)  
  {  
    if(i<j)  
    {  
      t=f[j];  
      f[j]=f[i];  
      f[i]=t;  
    }  
    k=N/2;  
    while(k<=j)  
    {  
      j=j-k;  
      k=k/2;  
    }  
    j=j+k;  
  }  
  
  /*----FFT算法----*/  
   for(m=1;m<=M;m++)  
  {
    la=pow(2.0,m); //la=2^m代表第m级每个分组所含节点数       
    lb=la/2;    //lb代表第m级每个分组所含碟形单元数  
                 //同时它也表示每个碟形单元上下节点之间的距离  
    /*----碟形运算----*/  
    for(l=1;l<=lb;l++)  
    {  
      r=(l-1)*pow(2.0,M-m);     
      for(n=l-1;n<N-1;n=n+la) //遍历每个分组，分组总数为N/la  
      {  
        lc=n+lb;  //n,lc分别代表一个碟形单元的上、下节点编号       
        Wn_i(N,r,&wn,1);//wn=Wnr  
        c_mul(f[lc],wn,&t);//t = f[lc] * wn复数运算  
        c_sub(f[n],t,&(f[lc]));//f[lc] = f[n] - f[lc] * Wnr  
        c_plus(f[n],t,&(f[n]));//f[n] = f[n] + f[lc] * Wnr  
      }  
    }  
  }  
} 
//傅里叶逆变换
//文件中的数据点是44100*2*15个，我们的目标是每12对（44100/8000=6）中，取每一对男生（6对）
//6对每一对按照8421变成float，并且做六对平均值作为一个数据，写入datai（44100/6*1*15=110250）	
ushort* load_wav(char *fname,uint32 w_size)
{      
	ushort *wbuf;
	FILE *fp;
	fp=fopen(fname,"rb");
        wbuf=(ushort *) malloc(w_size*2);
        fread(wbuf,sizeof(ushort),w_size,fp);
	return wbuf;
	fclose(fp);
}
int checkCrying(char* fname)
{  
	uint32 i,j,fs,tsize,rtmp,itmp,bound_j;
	uint32 bound[10];
	double afa;
	int crying;
        double bigsum,countsum,bound_avr,bound_var;
	bigsum=0.0;
	crying=0;
	countsum=0.0;
	bound_avr=0.0;
	bound_var=0.0;
	complex cwbuf[VN]={0,0};
	float cpow[VN]={0};
	ushort *wbuf;
	wbuf=load_wav(fname,size);
	tsize=VN*fs_div*path;
	if(size<tsize) { printf("unexpected result\n");free(wbuf);crying=0;return crying;}
	else
	{  rtmp=0;
	   itmp=0;
	   j=0;
		   for(i=0;i<tsize/2;i++)
		   {
			   rtmp+=wbuf[2*i];
			   itmp+=wbuf[2*i+1];
			   if((i+1)%fs_div==0) 
			   {
				   cwbuf[j].real=rtmp/(fs_div*65336.0);rtmp=0;
				   cwbuf[j++].imag=itmp/(fs_div*65336.0);itmp=0;
			   }
		   }
	   fft(VN,cwbuf);
	   c_pow(cwbuf,cpow,VN/2);
	   for(j=0;j<10;j++)
	   {
		   afa=0.56+j*0.01;
		   bigsum=0;
		   countsum=0;
	   for(i=1;i<VN/2;i++)
	   {
		   bigsum+=cpow[i];
	   }

	   bigsum=afa*bigsum;
	   for(i=1;i<VN/2;i++)
	   {
		   countsum+=cpow[i];
		   if(countsum>=bigsum) {bound[j]=i;break;}

	   }
	   }
	   bound_j=bound[4];
	   if(bound_j<=12000) crying=0;
	   else
	   {
	   for(j=0;j<9;j++)
	   {
		  bound[j]=bound[j+1]-bound[j];
	   }
	   for(j=0;j<9;j++)
		   bound_avr+=bound[j];
	   bound_avr/=9.0;
	   for(j=0;j<9;j++)
		   bound_var+=(bound[j]-bound_avr)*(bound[j]-bound_avr);
	   bound_var/=9.0;
	   printf("bound_var is %lf\n",bound_var);
	   if(7000<=bound_var&&bound_var<=50000)
	   {
		   crying=1;
		   printf("the baby is crying \n");
	   }
	   else 
            { crying=0;printf("the baby is not crying\n");}
	   }
          free(wbuf);
	 return crying;
	}
}
