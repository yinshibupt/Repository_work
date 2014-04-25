#include <stdio.h>
void swap(int n[],int i,int j)
{
	int temp = n[i];
	n[i]=n[j];
	n[j]=temp;
}
void colorsort(int n[],int length)
{
	if(length<=0) return;
	int i=0,j=length-1,k=0;
	while(i<j)
	{
		if(n[i]==2)
		{
			swap(n,i,j);
			j--;
		}
		else if(n[i]==0)
		{
			swap(n,k,i);
			k++;i++;
		}else i++;
	}
}
int main()
{
	int n[21],length;
	printf("input length of n[]\n");
	while(scanf("%d",&length)!=EOF)
	{
		for(int i=0;i<length;++i) scanf("%d",&n[i]);
		colorsort(n,length);
		for(int i=0;i<length;++i)
		{
			if(i!=length-1) printf("%d ", n[i]);
			else printf("%d\n", n[i]);
		} 
	}
	return 0;
}