package iqueryoptimizer

import (
	"iplan"
)

//nodeid globally save the nodeid
var relsites [4]int

func GetRelSites(tree iplan.PlanTree) [4]int {
	relsites[0] = 0
	relsites[1] = 0
	relsites[2] = 0
	relsites[3] = 0
	getRelSites(&tree, tree.Root)
	// println("GetRelCols finished, the tree is:")
	// newtree.Print()
	return relsites
}

func getRelSites(t *iplan.PlanTree, n int64) {
	node := &t.Nodes[n]
	if node.Nodeid == -1 {
		return
	}
	if node.Left != -1 {
		getRelSites(t, node.Left)
	}
	if node.Right != -1 {
		getRelSites(t, node.Right)
	}
	locate_id := node.Locate
	if locate_id > 3 {
		return
	}
	relsites[locate_id] = 1
	return
}
