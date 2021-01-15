package iqueryanalyzer

import (
	"fmt"
	"iparser"
	"iplan"
	"iutilities"
	"os"
	"strings"
)

var physicalPlanTree iplan.PlanTree

func splitWhere(where string) []string {
	arr := strings.Split(where, "and")
	result := []string{}
	for _, str := range arr {
		s := strings.Split(str, " ")
		if s[0] == "nation" && s[1] == "=" {
			result = append(result, str)
		}
	}
	return arr
}

func checkPublisherPrun() string {
	for _, node := range physicalPlanTree.Nodes {
		if node.NodeType == 2 {
			// relatedconditions := splitWhere(node.Where)

		}
	}
	return "no"
}

//GetFragTree return FragTree accroding to tablename
func GetFragTree(tableName string) (fragTree iplan.FragTree) {
	switch tableName {
	case "publisher":
		fragTree.TableName = "publisher"
		fragTree.FragType = 0
		// prunInfo := checkPublisherPrun()
		fragTree.FragNum = 4
		fragTree.Frags[0].FragId = 0
		fragTree.Frags[0].SiteNum = 0
		fragTree.Frags[0].FragName = "publisher_0"
		fragTree.Frags[1].FragId = 1
		fragTree.Frags[1].SiteNum = 1
		fragTree.Frags[1].FragName = "publisher_1"
		fragTree.Frags[2].FragId = 2
		fragTree.Frags[2].SiteNum = 2
		fragTree.Frags[2].FragName = "publisher_2"
		fragTree.Frags[3].FragId = 3
		fragTree.Frags[3].SiteNum = 3
		fragTree.Frags[3].FragName = "publisher_3"
	case "book":
		fragTree.TableName = "book"
		fragTree.FragNum = 3
		fragTree.FragType = 0
		fragTree.Frags[0].FragId = 0
		fragTree.Frags[0].SiteNum = 0
		fragTree.Frags[0].FragName = "book_0"
		fragTree.Frags[1].FragId = 1
		fragTree.Frags[1].SiteNum = 1
		fragTree.Frags[1].FragName = "book_1"
		fragTree.Frags[2].FragId = 2
		fragTree.Frags[2].SiteNum = 2
		fragTree.Frags[2].FragName = "book_2"
	case "customer":
		fragTree.TableName = "customer"
		fragTree.FragNum = 2
		fragTree.FragType = 1
		fragTree.Frags[0].FragId = 0
		fragTree.Frags[0].SiteNum = 0
		fragTree.Frags[0].FragName = "customer_0"
		fragTree.Frags[1].FragId = 1
		fragTree.Frags[1].SiteNum = 1
		fragTree.Frags[1].FragName = "customer_1"
	case "orders":
		fragTree.TableName = "orders"
		fragTree.FragNum = 4
		fragTree.FragType = 0
		fragTree.Frags[0].FragId = 0
		fragTree.Frags[0].SiteNum = 0
		fragTree.Frags[0].FragName = "orders_0"
		fragTree.Frags[1].FragId = 1
		fragTree.Frags[1].SiteNum = 1
		fragTree.Frags[1].FragName = "orders_1"
		fragTree.Frags[2].FragId = 2
		fragTree.Frags[2].SiteNum = 2
		fragTree.Frags[2].FragName = "orders_2"
		fragTree.Frags[3].FragId = 3
		fragTree.Frags[3].SiteNum = 3
		fragTree.Frags[3].FragName = "orders_3"
	}
	return fragTree
}

func findEmptyNode() int64 {
	for i, node := range physicalPlanTree.Nodes {
		if i != 0 && node.Nodeid == -1 {
			return int64(i)
		}
	}
	fmt.Println("Error when creating node, no empty node left!")
	return -1
}
func createTableNode(tableName string, siteNum int64) iplan.PlanTreeNode {
	node := iparser.InitalPlanTreeNode()
	node.NodeType = 1
	node.TmpTable = tableName
	node.Locate = siteNum

	return node
}
func addTableNode(newNode iplan.PlanTreeNode, root int64, fragType int64) int64 {
	if root == -1 {
		root = findEmptyNode()
		newNode.Nodeid = root
		physicalPlanTree.Nodes[root] = newNode
		physicalPlanTree.NodeNum++
	} else {
		pos := findEmptyNode()
		newNode.Nodeid = pos
		physicalPlanTree.Nodes[pos] = newNode
		physicalPlanTree.NodeNum++

		newroot := findEmptyNode()
		if fragType == 0 {
			physicalPlanTree.Nodes[newroot] = iparser.CreateUnionNode(iparser.GetTmpTableName())
			physicalPlanTree.NodeNum++
		} else {
			physicalPlanTree.Nodes[newroot] = iparser.CreateJoinNode(iparser.GetTmpTableName(), 2)
			physicalPlanTree.NodeNum++
		}

		physicalPlanTree.Nodes[newroot].Nodeid = newroot
		physicalPlanTree.Nodes[newroot].Left = root
		physicalPlanTree.Nodes[newroot].Right = pos
		physicalPlanTree.Nodes[pos].Parent = newroot
		physicalPlanTree.Nodes[root].Parent = newroot
		root = newroot

	}
	return root
}

func getChildType(id int64) string {
	if physicalPlanTree.Nodes[physicalPlanTree.Nodes[id].Parent].Left == id {
		return "Left"
	} else if physicalPlanTree.Nodes[physicalPlanTree.Nodes[id].Parent].Right == id {
		return "Right"
	}

	return "err"
}

func replace(old int64, new int64) {
	physicalPlanTree.Nodes[new].Parent = physicalPlanTree.Nodes[old].Parent
	switch getChildType(old) {
	case "Left":
		physicalPlanTree.Nodes[physicalPlanTree.Nodes[old].Parent].Left = new
	case "Right":
		physicalPlanTree.Nodes[physicalPlanTree.Nodes[old].Parent].Right = new
	default:
		fmt.Println("parent and child relationship is wrong")
	}
	physicalPlanTree.Nodes[old] = iparser.InitalPlanTreeNode()
	// physicalPlanTree.NodeNum--

}

//ShowPlanTree print tree
func ShowPlanTree(planTree iplan.PlanTree) {
	fmt.Printf("NodeNum is %d\n", planTree.NodeNum)
	fmt.Printf("root is %d\n", planTree.Root)
	for _, node := range planTree.Nodes {
		if node.Nodeid != -1 {
			fmt.Println(node)
		}
	}
}
func min(a int64, b int64) (min int64, info string) {
	iutilities.LoadAllConfig()
	clientSite := iutilities.GetMe().NodeId
	if a == b {
		min = a
		info = "Equal"
	} else if a == clientSite {
		min = a
		info = "Left"
	} else if b == clientSite {
		min = b
		info = "Right"
	} else if a < b {
		min = a
		info = "Left"
	} else if a > b {
		min = b
		info = "Right"
	}
	return min, info
}

func splitTableNode(tableNode iplan.PlanTreeNode) {
	fragTree := GetFragTree(tableNode.TmpTable)
	var root int64 = -1
	for i := int64(0); i < fragTree.FragNum; i++ {
		root = addTableNode(createTableNode(fragTree.Frags[i].FragName, fragTree.Frags[i].SiteNum), root, fragTree.FragType)
	}
	// ShowPlanTree(physicalPlanTree)
	replace(tableNode.Nodeid, root)
}
func getLocate(i int64) (locate int64) {
	if physicalPlanTree.Nodes[i].Locate != -1 {
		locate = physicalPlanTree.Nodes[i].Locate
	} else if physicalPlanTree.Nodes[i].Left != -1 && physicalPlanTree.Nodes[i].Right != -1 {
		//如果存在client站点，则优先client，否则取两个孩子中Locate小的那个
		var minchildinfo string
		locate, minchildinfo = min(getLocate(physicalPlanTree.Nodes[i].Left), getLocate(physicalPlanTree.Nodes[i].Right))
		switch minchildinfo {
		case "Left":
			physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Left].TransferFlag = false
			physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Right].TransferFlag = true
			physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Right].Dest = locate
		case "Right":
			physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Right].TransferFlag = false
			physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Left].TransferFlag = true
			physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Left].Dest = locate
		case "Equal":

		}
	} else if physicalPlanTree.Nodes[i].Left == -1 && physicalPlanTree.Nodes[i].Right != -1 {
		locate = getLocate(physicalPlanTree.Nodes[i].Right)

	} else if physicalPlanTree.Nodes[i].Left != -1 && physicalPlanTree.Nodes[i].Right == -1 {
		locate = getLocate(physicalPlanTree.Nodes[i].Left)
	} else {
		fmt.Println("error when getlocate, there is a node without child but don't have locate")
		fmt.Printf("nodeid is %d\n", i)
		os.Exit(1)
	}
	return locate
}

//Analyze can transer a logicalPlanTree to a physicalPlanTree
func Analyze(logicalPlanTree iplan.PlanTree) iplan.PlanTree {
	physicalPlanTree = logicalPlanTree
	for _, node := range physicalPlanTree.Nodes {
		if node.NodeType == 1 {
			splitTableNode(node)
		}
	}
	// ShowPlanTree(physicalPlanTree)
	for i, node := range physicalPlanTree.Nodes {
		if node.Nodeid == -1 {
			// fmt.Println("Continue..")
			continue
		} else if node.Locate == -1 {
			physicalPlanTree.Nodes[i].Locate = getLocate(int64(i))
		}
	}
	for i, node := range physicalPlanTree.Nodes {
		if node.Nodeid == -1 {
			continue
			// physicalPlanTree.Nodes[i].Nodeid = 0 //               !!!!!!!!
		} else if node.NodeType == 1 && node.TransferFlag == false {
			physicalPlanTree.Nodes[i].Status = 1
		} else {
			physicalPlanTree.Nodes[i].Status = 0
		}
	}
	// for i, node := range physicalPlanTree.Nodes {
	// 	if i != 0 && node.Nodeid == -1
	// }

	return physicalPlanTree
}
