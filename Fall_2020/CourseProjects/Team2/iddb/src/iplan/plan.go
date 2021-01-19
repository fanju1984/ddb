package iplan

import "fmt"

const (
	//MaxNodeNum define the max num of the plantree nodes
	MaxNodeNum = 1000
	MaxFragNum = 10
)

type PlanTreeNode struct {
	Nodeid       int64
	Left         int64
	Right        int64
	Parent       int64
	Status       int64  //1 for finished, 0 for waiting
	TmpTable     string //the name of tmp_table
	Locate       int64  // the sitenum,n for site_n
	TransferFlag bool   //1 for transer, 0 for not
	Dest         int64  //the sitenum of the dest
	NodeType     int64  //1 for table, 2 for select, 3 for projuection, 4 for join, 5 for union
	//detail string//according to node_type, (1)table_name for table, (2)where_condition for select, (3)col_name for projection, (4)join_type for join, (5)nil for union
	Where    string
	Rel_cols string //split(",",s)   !!!!
	Cols     string
	//cols_type string
	Joint_type int64  //0 for x, 1 for =, 2 for natural
	Joint_cols string //"customer_id,id"
	//union string
}
type PlanTree struct {
	NodeNum int64
	Root    int64 //add rootnum to find root !!!
	Nodes   [MaxNodeNum]PlanTreeNode
}

type FragNode struct {
	FragId        int64  //
	FragName      string //
	FragCondition string
	SiteNum       int64
	Ip            string
}

type FragTree struct {
	FragNum   int64
	FragType  int64 //0 for Horizontal, 1 for vertical
	TableName string
	Frags     [MaxFragNum]FragNode
}

func (pt *PlanTree) Print() {
	fmt.Println("root is: ", pt.Root)
	for _, node := range pt.Nodes {
		if node.Nodeid == -1 || node.Nodeid == 0 {
			continue
		}
		fmt.Println(node)
	}
	return
}
