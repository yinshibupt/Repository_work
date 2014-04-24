#include <stdio.h> 
#include <vector>
using namespace std;
int main()
{
	int n;
	while(scanf("%d",&n)!=EOF)
	{
		vector<int> v1;
		vector<int> v2;
		for(int i=0;i<n;++i) {scanf("%d",&num);v1.push_back(num);}
		for(vector<int>::iterator it=v1.begin();it!=v1.end();++it)
		{
			if((*it)%2==0) v2.push_back(*it);
			else printf("%d ",*it);
		}
		for(vector<int>::iterator it=v2.begin();it!=v2.end();++it)
		{
			if(it != v2.end()-1) printf("%d ",*it);
			else printf("%d\n",*it);
		}
	}
	return 0;
} 
