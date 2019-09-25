// 查询树的结构、可视化展示
package SQL

import (
	"fmt"
	//"strconv"
)

type Tree struct {
	node  *Node
	left  *Tree
	right *Tree
}
type Node struct {
	operation string
	condition string
}

func (node *Node) Print_Node() {
	fmt.Print(node.operation + " " + node.condition)
}

func NewNode(operation string, condition string) *Node {
	return &Node{
		operation: operation,
		condition: condition,
	}
}

func NewTree(node *Node) *Tree {
	return &Tree{
		node:  node,
		left:  nil,
		right: nil,
	}
}

var i int = 0

func CreateTree(T *Tree, arr []*Node, flag int) *Tree {
	//fmt.Print(i)
	//	fmt.Println(arr[i].operation + arr[i].condition)
	if arr[i].condition == "0" {
		T = nil
	} else {
		T = NewTree(arr[i])

		//fmt.Println(arr[i].operation)
		//		fmt.Println(arr[i].operation+arr[i].condition+strconv.Itoa(i))
		i++
		//fmt.Println("right")
		T.right = CreateTree(T.right, arr, i)

		i++
		//fmt.Println("left")
		T.left = CreateTree(T.left, arr, i)

		//fmt.Println(strconv.Itoa(i) + " " + arr[i].operation)
		//		fmt.Println(arr[i].operation+" "+arr[i].condition+" "+strconv.Itoa(i))

	}
	return T
}

func trans(root *Tree) {
	if root == nil {
		fmt.Println("Tree is nil!")
		return
	}
	//    fmt.Println(root) //前序遍历
	//fmt.Println("111")
	fmt.Println(root.node.operation + " : " + root.node.condition)
	//	fmt.Println(root.left.node.operation+" : "+root.left.node.condition)
	//	fmt.Println(root.right.node.operation+" : "+root.right.node.condition)
	trans(root.left)
	//fmt.Println(root) //中序遍历
	trans(root.right)
	//    fmt.Println(root.node.operation+" : "+root.node.condition)
	//	    fmt.Println(root) //后序遍历
}
func outputTree(root *Tree, left bool,s string) {
	if(root.right!=nil){
		if(left==true){
			outputTree(root.right,false,s + "|     ")
		}else{
			outputTree(root.right,false,s + "      ")
		}
	}
	fmt.Print(s)
	if(left==true){
		fmt.Print("\\")
	}else{
		fmt.Print("/")
	}
	fmt.Print("-----")
	if(root.left!=nil){
		fmt.Println(root.node.operation + " : " + root.node.condition)
	}
	if(root.left!=nil){
		if(left==false){
			outputTree(root.left,true,s + "|     ")
		}else{
			outputTree(root.left,true,s + "      ")
		}
	}
}
func trans_visual(root *Tree) {
	globalStack:=NewStack()
	globalStack.Push(root)
	var localStack *Stack
	nBlank:=32
	isRowEmpty:=false
	dot:="............................"
	fmt.Println(dot+dot+dot)
	for {
		if (isRowEmpty != false) {
			break
		} else {
			localStack = NewStack()
			isRowEmpty = true
			for j := 0; j < nBlank; j++ {
				fmt.Print("-")
			}
			for {
				if (globalStack.Empty() != false) {
					break
				} else {
					temp := globalStack.Pop().(*Tree)
					var x *Tree = nil
					if (temp != x) {
						temp1:=temp
						fmt.Print(temp1.node.operation + " : " + temp1.node.condition)
						localStack.Push(temp1.left)
						localStack.Push(temp1.right)
						if (temp1.left != nil || temp1.right != nil) {
							isRowEmpty = false
						}
					} else {
						fmt.Print("#!")
						localStack.Push(x)
						localStack.Push(x)
					}
					for j := 0; j < nBlank*2-2; j++ {
						fmt.Print(" ")
					}
				}
			}
			fmt.Println()
			nBlank /= 2
			for {
				if (localStack.Empty() == true) {
					break
				} else {
					globalStack.Push(localStack.Pop())
				}
			}
		}
	}
	fmt.Println(dot + dot + dot)

}
