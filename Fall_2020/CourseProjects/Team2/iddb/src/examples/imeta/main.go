package main

import (
	"fmt"
	"imeta"
	"iplan"

)


func main() {
	imeta.Connect_etcd()
	err := test_imeta()
	//err := test_frag()
	if( err!=nil ){
		fmt.Println("err")
	}
	//imeta.Disconnect_etcd()
}

func test_imeta()(error){	
	var tag int64
	for true{
		fmt.Println("1:Get_Tree; 2:Get_Node; 3:Set_Node;4: Build_txn; 5:Set_Tree 0: exit")
		fmt.Scanf("%d",&tag)
		//fmt.Println(tag)
		if (tag==1) {
			var nid int64
			fmt.Println("input txn id")
			fmt.Scanf("%d",&nid)
			//var treex iplan.PlanTree
			treex,err := imeta.Get_Tree(nid)
			if(err != nil){
				fmt.Println("err")
				return err
			}
			fmt.Println("--------------")
			fmt.Println(treex)
		}
		if (tag==2) {
			var tid int64
			var nid int64
			fmt.Println("input txn id, node id")
			fmt.Scanf("%d %d",&tid, &nid)
			//var nodex iplan.PlanTreeNode
			nodex,err := imeta.Get_Node(tid, nid)
			if(err != nil){
				fmt.Println("err")
				return err
			}
			fmt.Println("--------------")
			fmt.Println(nodex)
		}
		if (tag==3) {
			var tid int64
			var nid int64
			fmt.Println("input txn id, node id")
			fmt.Scanf("%d %d",&tid, &nid)
			var nodex iplan.PlanTreeNode
			nodex.Nodeid = int64(nid)
			nodex.Left = 2
			nodex.Right = 3
			nodex.Parent = 4
			nodex.Status = 5
			nodex.TmpTable = "tmp"
			nodex.Locate = 161
			nodex.TransferFlag = true
			nodex.Dest = 161
			nodex.NodeType = 2
			nodex.Where = "name=abc"
			nodex.Rel_cols = "id,name,dep"
			nodex.Cols = "name"
			nodex.Joint_type = 1
			nodex.Joint_cols = "id"
			//fmt.Println(nodex)
			err := imeta.Set_Node(tid, nodex)
			fmt.Println("--------------")
			if(err != nil){
				return err
			}else{
				fmt.Println("Put node success")
			}
		}
		if (tag==4){
			fmt.Println("build txn:")
			var tid int64
			fmt.Scanf("%d",&tid)
			err := imeta.Build_Txn(tid)
			if(err != nil){
				return err
			}
		}
		if (tag==5){
			fmt.Println("get tree:")
			var tid int64
			fmt.Scanf("%d",&tid)

			treex,err := imeta.Get_Tree(tid)
			if(err != nil){
				fmt.Println("err")
				return err
			}
			fmt.Println("--------------")

			fmt.Println("set tree:")
			var tidd int64
			fmt.Scanf("%d",&tidd)

			err = imeta.Set_Tree(tidd,treex)
			if(err != nil){
				return err
			}
		}
		if (tag==0){
			fmt.Println("bye bye")
			break
		}
	}
	return nil
}

/*
func test_frag()(error){	
	var tag int64
	for true{
		fmt.Println("1:Get_Tree; 2:Set_Tree;  0: exit")
		fmt.Scanf("%d",&tag)
		//fmt.Println(tag)
		if (tag == 1) {
			var tablename string
			fmt.Println("input tabelname")
			fmt.Scanf("%s",&tablename)
			fmt.Println(tablename)
			//var treex iplan.PlanTree
			treey,err := imeta.Get_FragTree(tablename)
			if(err != nil){
				fmt.Println("err")
				return err
			}
			fmt.Println("--------------")
			fmt.Println(treey)
		}
		if (tag == 2){
			FragN := 4
			var Treey iplan.FragTree
			Treey.FragNum = int64(FragN)
			i := 1
			for {
				Treey.Frags[i].FragId = int64(i)
				Treey.Frags[i].FragCondition = "id>1000"
				Treey.Frags[i].SiteNum = int64(i)
				Treey.Frags[i].Ip = "10.77.70.161"
				i=i+1
				if(i > FragN){
					break
				}
			}
			//fmt.Println("Tree in:")
			//fmt.Println(Treey)

			var tablename string
			fmt.Println("input tabelname")
			fmt.Scanf("%s",&tablename)
			err := imeta.Set_FragTree(tablename, Treey)
			if(err != nil){
				return err
			}
		}
		if (tag == 0){
			fmt.Println("bye bye")
			break
		}
	}
	return nil
}
*/


