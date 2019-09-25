// 查询树的形成访问
package SQL

import (
	"fmt"
)

//"fmt"

func Translate_C(parser *SQL_query) []*Node {

	var nodes []*Node = make([]*Node, 0)
	//fmt.Println(parser.tab.size)
	if parser.tab.size == 1 {
		var node1 *Node //nil
		var node2 *Node //T
		var node3 *Node //s
		var node4 *Node //p
		node1 = NewNode("0", "0")
		node2 = NewNode("T", parser.tab.tab[0].tab)
		var selTable string
		for j := 0; j < parser.sel.selT[0].size; j++ {
			selTable = selTable + parser.sel.selT[0].sel[j].attribute + parser.sel.selT[0].sel[j].operator + parser.sel.selT[0].sel[j].value + ","
		}
		node3 = NewNode("S", selTable)
		node4 = NewNode("P", parser.pro.pro[0])
		nodes = append(nodes, node1)
		nodes = append(nodes, node1)
		nodes = append(nodes, node2)
		nodes = append(nodes, node1)
		nodes = append(nodes, node3)
		nodes = append(nodes, node1)
		nodes = append(nodes, node4)

	} else if parser.tab.size != 0 {
		for i := 0; i < 2; i++ { //table 0&1
			var node1 *Node //nil
			var node2 *Node //T
			var node3 *Node //s
			var node4 *Node //p
			node1 = NewNode("0", "0")
			node2 = NewNode("T", parser.tab.tab[i].tab)
			var selTable string
			for j := 0; j < parser.sel.selT[i].size; j++ {
				selTable = selTable + parser.sel.selT[i].sel[j].attribute + parser.sel.selT[i].sel[j].operator + parser.sel.selT[i].sel[j].value + ","
			}
			node3 = NewNode("S", selTable)
			node4 = NewNode("P", parser.pro.pro[i])
			nodes = append(nodes, node1)
			nodes = append(nodes, node1)
			nodes = append(nodes, node2)
			nodes = append(nodes, node1)
			nodes = append(nodes, node3)
			nodes = append(nodes, node1)
			nodes = append(nodes, node4)
		}
		fmt.Println("-----------------step1: Create SQL parse tree-----------------")
		var node *Node
		node = NewNode("J", parser.jo.jo[0].tab1+" "+parser.jo.jo[0].tab2+" : "+parser.jo.jo[0].join_attr)
		nodes = append(nodes, node)
		for i := 2; i < parser.tab.size; i++ {
			var node1 *Node //nil
			var node2 *Node //T
			var node3 *Node //s
			var node4 *Node //p
			var node5 *Node //j
			node1 = NewNode("0", "0")
			node2 = NewNode("T", parser.tab.tab[i].tab)
			var selTable string
			for j := 0; j < parser.sel.selT[i].size; j++ {
				selTable = selTable + parser.sel.selT[i].sel[j].attribute + parser.sel.selT[i].sel[j].operator + parser.sel.selT[i].sel[j].value + ","
			}
			node3 = NewNode("S", selTable)
			node4 = NewNode("P", parser.pro.pro[i])
			node5 = NewNode("J", parser.jo.jo[i-1].tab1+" "+parser.jo.jo[i-1].tab2+" : "+parser.jo.jo[i-1].join_attr)
			nodes = append(nodes, node1)
			nodes = append(nodes, node1)
			nodes = append(nodes, node2)
			nodes = append(nodes, node1)
			nodes = append(nodes, node3)
			nodes = append(nodes, node1)
			nodes = append(nodes, node4)
			nodes = append(nodes, node5)
		}
		node = NewNode("0", "0")
		nodes = append(nodes, node)
		node = NewNode("P", parser.pro.pro[parser.tab.size])
		nodes = append(nodes, node)
	}

	//	fmt.Print(len(nodes))

	var nodes2 []*Node = make([]*Node, len(nodes)) //逆
	for key, value := range nodes {
		//		fmt.Print(value.condition)
		nodes2[len(nodes)-key-1] = value
	}

	//	fmt.Print(len(nodes2))

	return nodes2
}

/*
func Translate_S(parser *SQL_query) []*Node {

	var nodes []*Node = make([]*Node, 0)
	if parser.tab.size != 0 {
		for _, value := range parser.tab.tab {
			var node *Node
			var node1 *Node
			var node2 *Node
			node1 = NewNode("0", "0")
			node2 = NewNode("0", "0")
			node = NewNode("T", value.tab)
			nodes = append(nodes, node1)
			nodes = append(nodes, node2)
			nodes = append(nodes, node)
		}
	}
	if parser.jo.size != 0 {
		for _, value := range parser.jo.jo {
			var node *Node
			node = NewNode("J", value)
			nodes = append(nodes, node)
		}
	}
	if parser.sel.size != 0 {
		for _, value := range parser.sel.sel {
			var node1 *Node
			var node2 *Node
			node1 = NewNode("0", "0")
			node2 = NewNode("S", value)
			nodes = append(nodes, node1)
			nodes = append(nodes, node2)
		}
	}
	if parser.pro.size != 0 {
		for _, value := range parser.pro.pro {
			var node1 *Node
			var node2 *Node
			node1 = NewNode("0", "0")
			node2 = NewNode("P", value)
			nodes = append(nodes, node1)
			nodes = append(nodes, node2)
		}
	}
	//	fmt.Print(len(nodes))

	var nodes2 []*Node = make([]*Node, len(nodes)) //逆
	for key, value := range nodes {
		//		fmt.Print(value.condition)
		nodes2[len(nodes)-key-1] = value
	}

	//	fmt.Print(len(nodes2))

	return nodes2
}
*/
