package imeta

import (
	"context"
	"fmt"
	"iplan"
	"strconv"
	"strings"
	"time"

	"github.com/coreos/etcd/clientv3"
)

var Cli *clientv3.Client

func Connect_etcd() {
	config := clientv3.Config{
		Endpoints:   []string{"localhost:2379"},
		DialTimeout: 15 * time.Second,
	}
	var err error
	if Cli, err = clientv3.New(config); err != nil {
		fmt.Println(err.Error())
		return
	}
	fmt.Println("etcd connect succ")
}

// func Disconnect_etcd(){
// 	Cli.Close
// }

func Build_Txn(txnid int64) error {
	kv := clientv3.NewKV(Cli)
	node0 := "/" + strconv.FormatInt(txnid, 10)
	if putResp, err := kv.Put(context.TODO(), node0, "0##)", clientv3.WithPrevKV()); err != nil {
		fmt.Println(err)
		return err
	} else {
		if putResp.PrevKv != nil {
			fmt.Println("Not New")
			fmt.Println(string(putResp.PrevKv.Value))
		}
		fmt.Println("Build Txn " + node0 + " success")
		return nil
	}
}

func Get_Tree(txnid int64) (iplan.PlanTree, error) {
	kv := clientv3.NewKV(Cli)
	var treex string
	treex = strconv.FormatInt(txnid, 10)
	treex = "/" + treex + "/"
	if getResp, err := kv.Get(context.TODO(), treex, clientv3.WithPrefix()); err != nil {
		fmt.Println("err1")
		var a iplan.PlanTree
		return a, err
	} else {
		// fmt.Println("Get_Tree:")
		//fmt.Println(getResp.Kvs)
		var tree_return iplan.PlanTree
		var Nnum int64

		node0 := "/" + strconv.FormatInt(txnid, 10)
		if getResp, err := kv.Get(context.TODO(), node0); err != nil {
			fmt.Println("err3")
			var a iplan.PlanTree
			return a, err
		} else {
			s0 := string(getResp.Kvs[0].Value)
			sep0 := "##"
			value0 := strings.Split(s0, sep0)
			if len(value0) < 2 {
				fmt.Println("Plantree has no root or node num")
			}
			Nnum, err = strconv.ParseInt(value0[0], 10, 64)
			tree_return.NodeNum = Nnum
			tree_return.Root, err = strconv.ParseInt(value0[1], 10, 64)
		}
		for i, mykv := range getResp.Kvs {
			if i > int(Nnum) {
				break
			}
			//fmt.Println(strconv.FormatInt(i,10))
			//fmt.Println(string(mykv.Key))
			//fmt.Println(string(mykv.Value))

			s1 := string(mykv.Key)
			sep := "/"
			ikeys := strings.Split(s1, sep)
			// "/txnid/node id"
			ikey, err := strconv.ParseInt(ikeys[2], 10, 64)
			if err != nil {
				fmt.Println("err2")
				var b iplan.PlanTree
				return b, err
			}

			//split
			s2 := string(mykv.Value)
			sepp := "##"
			ivalues := strings.Split(s2, sepp)
			// 1##2##3##4##5##6##7##8##9##10##11##12##13##14##15
			if len(ivalues) != 15 {
				continue
			}
			//Nodes attr
			tree_return.Nodes[ikey].Nodeid, err = strconv.ParseInt(ivalues[0], 10, 64)
			tree_return.Nodes[ikey].Left, err = strconv.ParseInt(ivalues[1], 10, 64)
			tree_return.Nodes[ikey].Right, err = strconv.ParseInt(ivalues[2], 10, 64)
			tree_return.Nodes[ikey].Parent, err = strconv.ParseInt(ivalues[3], 10, 64)
			tree_return.Nodes[ikey].Status, err = strconv.ParseInt(ivalues[4], 10, 64)
			tree_return.Nodes[ikey].TmpTable = ivalues[5]
			tree_return.Nodes[ikey].Locate, err = strconv.ParseInt(ivalues[6], 10, 64)
			tree_return.Nodes[ikey].TransferFlag, err = strconv.ParseBool(ivalues[7])
			tree_return.Nodes[ikey].Dest, err = strconv.ParseInt(ivalues[8], 10, 64)
			tree_return.Nodes[ikey].NodeType, err = strconv.ParseInt(ivalues[9], 10, 64)
			tree_return.Nodes[ikey].Where = ivalues[10]
			tree_return.Nodes[ikey].Rel_cols = ivalues[11]
			tree_return.Nodes[ikey].Cols = ivalues[12]
			tree_return.Nodes[ikey].Joint_type, err = strconv.ParseInt(ivalues[13], 10, 64)
			tree_return.Nodes[ikey].Joint_cols = ivalues[14]
		}
		return tree_return, nil
	}
}

func Get_Node(txnid int64, nodeid int64) (iplan.PlanTreeNode, error) {
	kv := clientv3.NewKV(Cli)
	var nodex string
	nodex = "/" + strconv.FormatInt(txnid, 10) + "/" + strconv.FormatInt(nodeid, 10)
	if getResp, err := kv.Get(context.TODO(), nodex); err != nil {
		var a iplan.PlanTreeNode
		fmt.Println(err)
		return a, err
	} else {
		//fmt.Println("Get_NODE:")
		//fmt.Println(getResp.Kvs)

		var Node_return iplan.PlanTreeNode

		//split
		s2 := string(getResp.Kvs[0].Value)
		sepp := "##"
		ivalues := strings.Split(s2, sepp)
		//ivalue -> Node
		// 1##2##3##4##5##6##7##8##9##10##11##12##13##14##15
		if len(ivalues) != 15 {
			fmt.Println("node " + nodex + "has wrong  attribute number:" + strconv.Itoa(len(ivalues)))
			var a iplan.PlanTreeNode
			//fmt.Println(err)
			return a, err
		}
		//Nodes attr
		Node_return.Nodeid, err = strconv.ParseInt(ivalues[0], 10, 64)
		Node_return.Left, err = strconv.ParseInt(ivalues[1], 10, 64)
		Node_return.Right, err = strconv.ParseInt(ivalues[2], 10, 64)
		Node_return.Parent, err = strconv.ParseInt(ivalues[3], 10, 64)
		Node_return.Status, err = strconv.ParseInt(ivalues[4], 10, 64)
		Node_return.TmpTable = ivalues[5]
		Node_return.Locate, err = strconv.ParseInt(ivalues[6], 10, 64)
		Node_return.TransferFlag, err = strconv.ParseBool(ivalues[7])
		Node_return.Dest, err = strconv.ParseInt(ivalues[8], 10, 64)
		Node_return.NodeType, err = strconv.ParseInt(ivalues[9], 10, 64)
		Node_return.Where = ivalues[10]
		Node_return.Rel_cols = ivalues[11]
		Node_return.Cols = ivalues[12]
		Node_return.Joint_type, err = strconv.ParseInt(ivalues[13], 10, 64)
		Node_return.Joint_cols = ivalues[14]

		return Node_return, nil
	}
}

func Set_Node(txnid int64, Node_in iplan.PlanTreeNode) error {
	kv := clientv3.NewKV(Cli)
	//key value
	if int(Node_in.Nodeid) < 0 {
		Node_in.Nodeid = int64(0)
	}
	ikey := "/" + strconv.FormatInt(txnid, 10) + "/" + strconv.FormatInt(Node_in.Nodeid, 10)
	var ivalue string
	ivalue = strconv.FormatInt(Node_in.Nodeid, 10) + "##" + strconv.FormatInt(Node_in.Left, 10) + "##" + strconv.FormatInt(Node_in.Right, 10) + "##"
	ivalue = ivalue + strconv.FormatInt(Node_in.Parent, 10) + "##" + strconv.FormatInt(Node_in.Status, 10) + "##" + Node_in.TmpTable + "##" + strconv.FormatInt(Node_in.Locate, 10) + "##"
	ivalue = ivalue + strconv.FormatBool(Node_in.TransferFlag) + "##" + strconv.FormatInt(Node_in.Dest, 10) + "##" + strconv.FormatInt(Node_in.NodeType, 10) + "##"
	ivalue = ivalue + Node_in.Where + "##" + Node_in.Rel_cols + "##" + Node_in.Cols + "##" + strconv.FormatInt(Node_in.Joint_type, 10) + "##" + Node_in.Joint_cols

	if putResp, err := kv.Put(context.TODO(), ikey, ivalue, clientv3.WithPrevKV()); err != nil {
		fmt.Println(err)
		return err
	} else {
		if putResp.PrevKv != nil {
			fmt.Println("Update:")
			fmt.Println(string(putResp.PrevKv.Value))
		}
		//iplan.PlanTree NodeNum at /txnid
		node0 := "/" + strconv.FormatInt(txnid, 10)
		if getResp, err := kv.Get(context.TODO(), node0); err != nil {
			fmt.Println(err)
			return err
		} else {
			s0 := string(getResp.Kvs[0].Value)
			sep0 := "##"
			value0 := strings.Split(s0, sep0)
			if len(value0) < 2 {
				fmt.Println("Plantree has no root or node num")
			}
			var oldnum int64
			oldnum, err = strconv.ParseInt(value0[0], 10, 64)
			if Node_in.Nodeid > oldnum {
				newnum := strconv.FormatInt(Node_in.Nodeid, 10)
				value00 := newnum + "##" + value0[1]
				kv.Put(context.TODO(), node0, value00, clientv3.WithPrevKV())
			}

		}
	}
	return nil
}

func Set_Tree(txnid int64, Tree_in iplan.PlanTree) error {
	kv := clientv3.NewKV(Cli)

	//put NodeNum##Root
	node0 := "/" + strconv.FormatInt(txnid, 10)
	value0 := strconv.FormatInt(Tree_in.NodeNum, 10) + "##" + strconv.FormatInt(Tree_in.Root, 10)
	if putResp, err := kv.Put(context.TODO(), node0, value0, clientv3.WithPrevKV()); err != nil {
		fmt.Println(err)
		return err
	} else {
		if putResp.PrevKv != nil {
			fmt.Println("Not New")
			fmt.Println(string(putResp.PrevKv.Value))
		}
		//fmt.Println("Build Txn "+node0+" success")
	}
	// fmt.Println("Tree_in:")
	// fmt.Println(Tree_in)

	for i, node_in := range Tree_in.Nodes {

		err := Set_Node(txnid, node_in)
		if err != nil {
			return err
		}
		if i >= int(Tree_in.NodeNum) {
			break
		}
	}
	fmt.Println("Set Tree success")

	return nil

}
