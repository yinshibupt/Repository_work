/****************************************************
    产生N=100个在（0,50）区间内满足对数正态分布的随机数
/*****************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#define N 10000 
#define MAX 100
#define MIN -100
#define MIU 0
#define SIGMA 5
#define PI 3.14159

 
double AverageRandom(double min,double max)//产生(min,max)之间均匀分布的随机数
{
        int MINnteger = (int)(min*10000);
        int MAXnteger = (int)(max*10000);
        int randInteger = rand()*rand();
    
        int diffInteger = MAXnteger - MINnteger;
        int resultInteger = randInteger % diffInteger + MINnteger;
        return resultInteger/10000.0;
}
double LogNormal(double x,double miu,double sigma) //对数正态分布概率密度函数
{
  //      return 1.0/(x*sqrt(2*PI)*sigma) * exp(-1*(log(x)-miu)*(log(x)-miu)/(2*sigma*sigma));
        return 1.0/sqrt(2*PI*sigma) * exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
}
double Random_LogNormal(double miu,double sigma,double min,double max)//产生对数正态分布随机数
{
        double x;
        double dScope;
        double y;
        do
        {
        x = AverageRandom(min,max); 
        //f+=x;
        y = LogNormal(x, miu, sigma);//产生正态分布 
        dScope = AverageRandom(0, LogNormal(miu,miu,sigma));
        }while( dScope > y);
        return x;
}
int main(void)
{
    srand(time(NULL));
    int i,j,n;
    while(scanf("%d",&n)!=EOF)
    {
    	while(n--)
    	{
		    float f=0;
		    for(i=0,j=0;i<N;i++)
		    {
		    	float temp=Random_LogNormal(MIU,SIGMA,MIN,MAX);
		    	f+=temp;
		        printf("%14f", temp);
		        j++;
		        if(j==5)
		        {
		            printf("\n");    //每行显示5个数
		            j=0;
		        }
		    }
		    printf("\nf is %f,N is %d\n",f,N);
		    printf("avg is %f\n",f/N);
		}
	}
    return 0;
}
