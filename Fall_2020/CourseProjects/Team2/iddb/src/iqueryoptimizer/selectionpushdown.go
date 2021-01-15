package iqueryoptimizer

import (
	// "fmt"
	"iparser"
	"iplan"
	"strings"
)

func JointConditionPushDown(oldtree iplan.PlanTree) iplan.PlanTree {
	var newtree iplan.PlanTree
	newtree = oldtree
	return newtree
}

var pt iplan.PlanTree

func isCol(str string) bool {
	var cols = []string{"pid", "pname", "nation", "bid", "title", "authors", "bpid", "copies", "cid", "cname", "rank", "ocid", "obid", "quantity"}
	for _, col := range cols {
		if str == col {
			return true
		}
	}
	return false
}

func isInclude(strin string, arr []string) bool {
	for _, str := range arr {
		if strin == str {
			return true
		}
	}
	return false
}

//subwhere里包含的col是否在relcols中也全部都有
func checkCols(parentWhere string, childrelColsinComma string) bool {
	parentRelCols := splitRelCols(parentWhere)
	childRelCols := strings.Split(childrelColsinComma, ",")
	for _, pcol := range parentRelCols {
		if isInclude(pcol, childRelCols) == false {
			return false
		}
	}
	return true
}

//findEmptyNode will return the idx of first empty node
func findEmptyNode() int64 {
	for i, node := range pt.Nodes {
		if i != 0 && node.Nodeid == -1 {
			return int64(i)
		}
	}
	println("Error when creating node, no empty node left!")
	return -1
}

func getChildType(childid int64) string {
	if pt.Nodes[pt.Nodes[childid].Parent].Left == childid {
		return "Left"
	} else if pt.Nodes[pt.Nodes[childid].Parent].Right == childid {
		return "Right"
	}

	return "err"
}

//把节点加在输入节点的上方
func addWhereNodeOnTop(newNode iplan.PlanTreeNode, nodeid int64) {
	if pt.Nodes[nodeid].NodeType == 4 && pt.Nodes[nodeid].Joint_type == 0 { // x join
		pt.Nodes[nodeid].Joint_type = 1
		pt.Nodes[nodeid].Where = newNode.Where
	} else {
		newNodeid := findEmptyNode()
		newNode.Nodeid = newNodeid
		newNode.Parent = pt.Nodes[nodeid].Parent
		newNode.Left = nodeid
		newNode.Locate = pt.Nodes[nodeid].Locate
		newNode.TransferFlag = pt.Nodes[nodeid].TransferFlag
		newNode.Dest = pt.Nodes[nodeid].Dest

		if pt.Nodes[nodeid].TransferFlag == true {
			pt.Nodes[nodeid].TransferFlag = false
			pt.Nodes[nodeid].Dest = -1
		}
		switch getChildType(nodeid) {
		case "Left":
			pt.Nodes[pt.Nodes[nodeid].Parent].Left = newNodeid
		case "Right":
			pt.Nodes[pt.Nodes[nodeid].Parent].Right = newNodeid
		case "err":
			println("Error: childType Error")
		}

		pt.Nodes[nodeid].Parent = newNodeid
		pt.Nodes[newNodeid] = newNode //向数组中加入节点
		pt.NodeNum++
		pt = GetRelCols(pt)
	}
}

func deleteWhereNode(nodeid int64) {
	node := pt.Nodes[nodeid]
	pt.Nodes[node.Parent].Left = node.Left
	pt.Nodes[node.Left].Parent = node.Parent
	pt.Nodes[nodeid] = iparser.InitalPlanTreeNode()
}

func find2ChildNode(curNode int64) (pos int64) {

	for node := pt.Nodes[curNode]; ; node = pt.Nodes[node.Left] {
		if node.Left != -1 && node.Right != -1 {
			return node.Nodeid
		} else if node.Left == -1 && node.Right == -1 {
			return -1
		}

	}
}

func tryPushDown(subWhere string, beginNode int64) {
	pos := find2ChildNode(beginNode)
	if pos == -1 { //若为-1则说明没有两个孩子的节点，只能加在curPos上
		addWhereNodeOnTop(iparser.CreateSelectionNode(iparser.GetTmpTableName(), subWhere), beginNode)
	} else {
		flag1 := checkCols(subWhere, pt.Nodes[pt.Nodes[pos].Left].Rel_cols)
		flag2 := checkCols(subWhere, pt.Nodes[pt.Nodes[pos].Right].Rel_cols)
		if flag1 == false && flag2 == false {
			addWhereNodeOnTop(iparser.CreateSelectionNode(iparser.GetTmpTableName(), subWhere), pos)
		}
		if flag1 == true {
			tryPushDown(subWhere, pt.Nodes[pos].Left)
		}
		if flag2 == true {
			tryPushDown(subWhere, pt.Nodes[pos].Right)
		}
	}
}

func splitRelCols(subWhere string) []string {
	arr := strings.Split(subWhere, " ")
	ColCount := 0
	reCols := []string{}
	for _, str := range arr {
		if isCol(str) {
			ColCount++
			reCols = append(reCols, str)
		}
	}
	return reCols
}

func SelectionPushDown(oldtree iplan.PlanTree) iplan.PlanTree {
	pt = oldtree
	for _, node := range pt.Nodes {
		if node.NodeType == 2 {
			//按照and分割where子句
			//方法：先按照空格分割，然后检测and来组合
			wheres := strings.Split(node.Where, "and")
			for _, subWhere := range wheres {
				subWhere = "where " + subWhere
				tryPushDown(subWhere, node.Nodeid)
			}
			deleteWhereNode(node.Nodeid)
		}

	}

	for i, node := range pt.Nodes {
		if node.NodeType == 1 && node.TransferFlag == false {
			pt.Nodes[i].Status = 1
		} else {
			pt.Nodes[i].Status = 0
		}
	}

	pt.Print()
	return pt
}
