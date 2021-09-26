#include <vector>
#include "iostream"
#include <stdio.h>
#include <string>

#include <set>
#include <iterator>
#include <algorithm>

using namespace std;


// //容器vector中元素的去重
// vector<int> unique_element_in_vector(vector<int> v){
//     vector<int>::iterator vector_iterator;
//     sort(v.begin(),v.end());
//     vector_iterator = unique(v.begin(),v.end());
//     if(vector_iterator != v.end()){
//         v.erase(vector_iterator,v.end());
//     }
//     return v;
// }

struct NODE {
	int id;
	vector<int> child;
	int parent;
	string sql_statement;
	int site;
};
struct TREE {
    int tree_id;
    int root;
    vector<NODE> Nodes;
};

/* 输入一个树结构，返回这棵树的根节点 */
NODE get_root(TREE tree){
    int root_id = tree.root;
    int i;
    NODE root_node;
    root_node.id = -1;
    for(i=0; i<tree.Nodes.size(); i++){
        if(tree.Nodes[i].id == root_id){
            root_node = tree.Nodes[i];
            return root_node;
        }
    }
    if(root_node.id == -1){
        printf("root node does not exist!");
    }
}

/* 输入一个树结构和节点id，返回以对应节点为根节点的子树，也是个递归函数 */
TREE get_sub_tree(TREE tree, int node_id){
    TREE sub_tree;
    sub_tree.tree_id = tree.tree_id;
    sub_tree.root = -1;
    vector<NODE> sub_nodes;
    /* 判断是否是叶子节点 */
    for(int i=0; i<tree.Nodes.size(); i++){
        if(tree.Nodes[i].id == node_id){
            sub_tree.root = node_id;
            NODE tmp_node = tree.Nodes[i];
            sub_nodes.push_back(tmp_node);
            /* 如果是叶子节点，就只放根节点进去然后返回 */
            if(tree.Nodes[i].child.size() == 0){
                printf("leaf node id %d\n", tree.Nodes[i].id);
                sub_tree.Nodes = sub_nodes;
                return sub_tree;
            }
            else{
                /* 如果不是叶子节点，就找它的孩子代表的子树，合并 */
                vector<int> childs_id = tree.Nodes[i].child;
                for(int k=0; k<childs_id.size(); k++){
                    TREE child_sub_tree = get_sub_tree(tree, childs_id[k]);
                    /* 把子树的节点都加入本树的节点 */
                    for(int j=0; j<child_sub_tree.Nodes.size(); j++){
                        sub_nodes.push_back(child_sub_tree.Nodes[j]);
                    }
                }
                /* 去除重复的，只是为了验证别的地方对不对，真正的树应当没有重复的内容 */
                vector<NODE> sub_nodes_unique;
                int flag = 0;
                for(NODE tmp_node: sub_nodes){
                    flag = 0;
                    for(NODE tmp_node_unique: sub_nodes_unique){
                        if(tmp_node.id == tmp_node_unique.id){
                            /* 之前出现过 */
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0){
                        /* 之前没出现过，加入列表 */
                        sub_nodes_unique.push_back(tmp_node);
                    }
                }
                sub_tree.Nodes = sub_nodes_unique;
                return sub_tree;
            }
            
        }      
    }
    if(sub_tree.root == -1){
        printf("this node does not exist!\n");
        sub_tree.Nodes = sub_nodes;
        return sub_tree;
    }
}

int main() {
    // example
    NODE node;
    vector<NODE> nodes;
    TREE Tree;
    Tree.tree_id = 0;
    Tree.root = 24;

    node.id = 1;
    node.parent = 2;
    node.sql_statement = "select * from Customer_1";
    node.site = 1;
    nodes.push_back(node);

    node.id = 2;
    node.child.clear();
    node.child.push_back(1);
    node.parent = 23;
    node.sql_statement = "select * from tree_0_node_1 where id > 308000";
    node.site = 1;
    nodes.push_back(node);

    node.id = 3;
    node.child.clear();
    node.parent = 6;
    node.sql_statement = "select * from Book_1";
    node.site = 1;
    nodes.push_back(node);

    node.id = 4;
    node.child.clear();
    node.parent = 7;
    node.sql_statement = "select * from Book_2";
    node.site = 2;
    nodes.push_back(node);

    node.id = 5;
    node.child.clear();
    node.parent = 8;
    node.sql_statement = "select * from Book_3";
    node.site = 3;
    nodes.push_back(node);

    node.id = 6;
    node.child.clear();
    node.child.push_back(3);
    node.parent = 15;
    node.sql_statement = "select * from tree_0_node_3 where copies > 100";
    node.site = 1;
    nodes.push_back(node);

    node.id = 7; // 多 parent
    node.child.clear();
    node.child.push_back(4);
    node.parent = 16;
    node.sql_statement = "select * from tree_0_node_4 where copies > 100";
    node.site = 2;
    nodes.push_back(node);

    node.id = 8;
    node.child.clear();
    node.child.push_back(5);
    node.parent = 17;
    node.sql_statement = "select * from tree_0_node_5 where copies > 100";
    node.site = 3;
    nodes.push_back(node);

    node.id = 9;
    node.child.clear();
    node.parent = 20;
    node.sql_statement = "select * from Publisher_1";
    node.site = 1;
    nodes.push_back(node);

    node.id = 10;
    node.child.clear();
    node.parent = 21;
    node.sql_statement = "select * from Publisher_2";
    node.site = 2;
    nodes.push_back(node);

    node.id = 11;
    node.child.clear();
    node.parent = 13;
    node.sql_statement = "select * from Order_3";
    node.site = 3;
    nodes.push_back(node);

    node.id = 12;
    node.child.clear();
    node.parent = 14;
    node.sql_statement = "select * from Order_4";
    node.site = 4;
    nodes.push_back(node);

    node.id = 13; // 多parent
    node.child.clear();
    node.child.push_back(11);
    node.parent = 15;
    node.sql_statement = "select * from tree_0_node_11 where quantity > 1";
    node.site = 3;
    nodes.push_back(node);

    node.id = 14;
    node.child.clear();
    node.child.push_back(12);
    node.parent = 18;
    node.sql_statement = "select * from tree_0_node_12 where quantity > 1";
    node.site = 4;
    nodes.push_back(node);

    node.id = 15;
    node.child.clear();
    node.child.push_back(6);
    node.child.push_back(13);
    node.parent = 19;
    node.sql_statement = "select * from tree_0_node_6,tree_0_node_13 where tree_0_node_6.id = tree_0_node_13.book_id";
    node.site = 1;
    nodes.push_back(node);

    node.id = 16;
    node.child.clear();
    node.child.push_back(7);
    node.child.push_back(13);
    node.parent = 19;
    node.sql_statement = "select * from tree_0_node_7,tree_0_node_13 where tree_0_node_7.id = tree_0_node_13.book_id";
    node.site = 2;
    nodes.push_back(node);

    node.id = 17;
    node.child.clear();
    node.child.push_back(8);
    node.child.push_back(13);
    node.parent = 19;
    node.sql_statement = "select * from tree_0_node_8,tree_0_node_13 where tree_0_node_8.id = tree_0_node_13.book_id";
    node.site = 3;
    nodes.push_back(node);

    node.id = 18;
    node.child.clear();
    node.child.push_back(8);
    node.child.push_back(14);
    node.parent = 19;
    node.sql_statement = "select * from tree_0_node_8,tree_0_node_14 where tree_0_node_8.id = tree_0_node_14.book_id";
    node.site = 3;
    nodes.push_back(node);

    node.id = 19; // 多parent
    node.child.clear();
    node.child.push_back(15);
    node.child.push_back(16);
    node.child.push_back(17);
    node.child.push_back(18);
    node.parent = 20;
    node.sql_statement = "select * from tree_0_node_15 UNION ALL select * from tree_0_node_16 UNION ALL select * from tree_0_node_17 UNION ALL select * from tree_0_node_18";
    node.site = 1;
    nodes.push_back(node);

    node.id = 20;
    node.child.clear();
    node.child.push_back(19);
    node.child.push_back(9);
    node.parent = 22;
    node.sql_statement = "select * from tree_0_node_19,tree_0_node_9 where tree_0_node_19.publisher_id = tree_0_node_9.id";
    node.site = 1;
    nodes.push_back(node);

    node.id = 21;
    node.child.clear();
    node.child.push_back(19);
    node.child.push_back(10);
    node.parent = 22;
    node.sql_statement = "select * from tree_0_node_19,tree_0_node_10 where tree_0_node_19.publisher_id = tree_0_node_10.id";
    node.site = 1;
    nodes.push_back(node);

    node.id = 22;
    node.child.clear();
    node.child.push_back(20);
    node.child.push_back(21);
    node.parent = 23;
    node.sql_statement = "select * from tree_0_node_20 UNION ALL select * from tree_0_node_21";
    node.site = 1;
    nodes.push_back(node);

    node.id = 23;
    node.child.clear();
    node.child.push_back(22);
    node.child.push_back(2);
    node.parent = 24;
    node.sql_statement = "select * from tree_0_node_22,tree_0_node_2 where tree_0_node_22.customer_id = tree_0_node_2.id";
    node.site = 1;
    nodes.push_back(node);

    node.id = 24;
    node.child.clear();
    node.child.push_back(23);
    node.parent = -1;
    node.sql_statement = "select Customer.name, Book.title, Publisher.name, Orders.quantity from tree_0_node_23";
    node.site = 1;
    nodes.push_back(node);

    node.id = 25;
    node.child.clear();
    node.child.push_back(4);
    node.parent = 18;
    node.sql_statement = "select * from tree_0_node_4 where copies > 100";
    node.site = 2;
    nodes.push_back(node);

    node.id = 26; // 多parent
    node.child.clear();
    node.child.push_back(11);
    node.parent = 16;
    node.sql_statement = "select * from tree_0_node_11 where quantity > 1";
    node.site = 3;
    nodes.push_back(node);

    node.id = 13; // 多parent
    node.child.clear();
    node.child.push_back(11);
    node.parent = 17;
    node.sql_statement = "select * from tree_0_node_11 where quantity > 1";
    node.site = 3;
    nodes.push_back(node);

    node.id = 27; // 多parent
    node.child.clear();
    node.child.push_back(15);
    node.child.push_back(16);
    node.child.push_back(17);
    node.child.push_back(18);
    node.parent = 21;
    node.sql_statement = "select * from tree_0_node_15 UNION ALL select * from tree_0_node_16 UNION ALL select * from tree_0_node_17 UNION ALL select * from tree_0_node_18";
    node.site = 1;
    nodes.push_back(node);

    Tree.Nodes = nodes;
    // NODE new_node = get_root(Tree);
    // printf("%d\n", Tree.root);
    // printf("%d\n", new_node.id);

    TREE sub_tree = get_sub_tree(Tree, 24);
    for(int i=0; i<sub_tree.Nodes.size(); i++){
        printf("node id:%d\n", sub_tree.Nodes[i].id);
    }
    int p = sub_tree.Nodes.size();
    printf("length %d\n", p);
    return 0;
}