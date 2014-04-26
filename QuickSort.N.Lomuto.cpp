#include <stdio.h>
void swap(int *a,int *b)
{
	int temp=*a;
	*a=*b;
	*b=temp;
}
int partition(int A[],int b,int e)
{
	int i=b-1;
	int x=A[e];
	for(int cur=b;cur<e;++cur) if(A[cur]<=x) swap(&A[++i],&A[cur]);
	swap(&A[i+1],&A[e]);
	return i+1;
}
void quicksort(int A[],int begin,int end)
{
	if(begin<end)
	{
		int partition_k = partition(A,begin,end);
		quicksort(A,begin,partition_k-1);
		quicksort(A,partition_k+1,end);
	}
}
int main(int argc, char const *argv[])
{
	int length,A[101];
	while(scanf("%d",&length)!=EOF)
	{
		for(int i=0;i<length;++i) scanf("%d",&A[i]);
		printf("Before quicksort, arr is:\n");
		for(int i=0;i<length;++i) 
		{
			if(i==length-1) printf("%d\n",A[i]);
			else printf("%d ",A[i]);
		}
		printf("\n*****************************\n");
		quicksort(A,0,length-1);
		printf("After quicksort, arr is:\n");
		for(int i=0;i<length;++i) 
		{
			if(i==length-1) printf("%d\n",A[i]);
			else printf("%d ",A[i]);
		}
	}
	return 0;
}