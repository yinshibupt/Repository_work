/**
*  Convert Sorted Array to Binary Search Tree
*Given an array where elements are sorted in ascending order, convert it to a height balanced BST.
*/

/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    TreeNode *treebulid(vector<int> &num,int i,int j)
    {
        if(i>j) return NULL;
        else
        {
            int mid = (j+i)/2;
            TreeNode* node = (TreeNode *)malloc(sizeof(TreeNode)); 
            node->val = num[mid];
            node->left = treebulid(num,i,mid-1);
            node->right = treebulid(num,mid+1,j);
            return node;
        }
    }
    TreeNode *sortedArrayToBST(vector<int> &num) {
        if(num.size()==0) return NULL;
        return treebulid(num,0,num.size()-1);
    }
};