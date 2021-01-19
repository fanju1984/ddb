package iqueryoptimizer

import (
	"iplan"
	"strings"
)

//nodeid globally save the nodeid

func GetRelCols(oldtree iplan.PlanTree) iplan.PlanTree {
	var newtree iplan.PlanTree
	getRelCols(&oldtree, oldtree.Root)
	newtree = oldtree
	// println("GetRelCols finished, the tree is:")
	// newtree.Print()
	return newtree
}

func getRelCols(t *iplan.PlanTree, n int64) {
	//if invalid Nodeid
	node := &t.Nodes[n]
	// println("!!!!", node.Nodeid)
	if node.Nodeid == -1 {
		return
	}
	//if access leaf node
	if node.Left == -1 && node.Right == -1 {
		getleafcols(node)
		// println(node.Rel_cols)
	}
	//if only n.Left exist
	if node.Left != -1 && node.Right == -1 {
		getRelCols(t, node.Left)
		node.Rel_cols = t.Nodes[node.Left].Rel_cols
		// println(node.Rel_cols)
	}
	//if only n.Right exist
	if node.Right != -1 && node.Left == -1 {
		getRelCols(t, node.Right)
		node.Rel_cols = t.Nodes[node.Right].Rel_cols
		// println(node.Rel_cols)
	}

	if node.Left != -1 && node.Right != -1 {
		getRelCols(t, node.Left)
		getRelCols(t, node.Right)
		// println(t.Nodes[node.Left].Rel_cols)
		// println(t.Nodes[node.Right].Rel_cols)
		node.Rel_cols = union(t.Nodes[node.Left].Rel_cols, t.Nodes[node.Right].Rel_cols)
		// println(node.Rel_cols)
	}

	if node.NodeType == 3 {
		node.Rel_cols = node.Cols
	}

	return
}

func getleafcols(node *iplan.PlanTreeNode) {

	if strings.Contains(node.TmpTable, "publisher") {
		node.Rel_cols = "pid,pname,nation"
	} else if strings.Contains(node.TmpTable, "book") {
		node.Rel_cols = "bid,title,authors,bpid,copies"
	} else if strings.Contains(node.TmpTable, "customer_0") {
		node.Rel_cols = "cid,cname"
	} else if strings.Contains(node.TmpTable, "customer_1") {
		node.Rel_cols = "cid,rank"
	} else if strings.Contains(node.TmpTable, "orders") {
		node.Rel_cols = "ocid,obid,quantity"
	}
	return
}

func union(str1 string, str2 string) string {
	f := func(c rune) bool {
		if c == ',' {
			return true
		}
		return false
	}
	// println(str1)
	// println(str2)
	arr1 := strings.FieldsFunc(str1, f)
	arr2 := strings.FieldsFunc(str2, f)
	var arr3 []string
	for _, str := range arr1 {
		arr3 = append(arr3, str)
	}
	for _, str := range arr2 {
		if notExist(arr3, str) {
			// println(arr3, str)
			arr3 = append(arr3, str)
		}
	}
	result := ""
	length := len(arr3)
	for i, str := range arr3 {
		result += str
		if i < length-1 {
			result += ","
		}
	}

	return result
}

func notExist(arr []string, s string) bool {
	for _, str := range arr {
		if strings.EqualFold(str, s) {
			return false
		}
	}
	return true
}
