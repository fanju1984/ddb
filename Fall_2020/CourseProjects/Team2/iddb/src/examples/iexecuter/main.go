package main

import (
	"database/sql"
	"fmt"
	"iexec"
	"iplan"

	_ "github.com/go-sql-driver/mysql"
)

func main() {
	test_mysql()
	// test_tree()
}

func test_mysql() {
	fmt.Println("try to connect mysql")
	db, err := sql.Open("mysql", "root:123456@tcp(127.0.0.1:3306)/iddb?charset=utf8")
	checkErr(err)

	// insert
	fmt.Println("try to insert")
	stmt, err := db.Prepare("INSERT customer SET id=?,name=?")
	checkErr(err)

	res, err := stmt.Exec(1, "zhhy")
	checkErr(err)

	// update
	fmt.Println("try to update")
	stmt, err = db.Prepare("update customer set name=? where id=?")
	checkErr(err)

	res, err = stmt.Exec("zhhy2", 1)
	checkErr(err)

	affect, err := res.RowsAffected()
	checkErr(err)

	fmt.Println(affect)

	// query
	fmt.Println("try to select first")
	rows, err := db.Query("SELECT * FROM customer")
	checkErr(err)

	for rows.Next() {
		var uid sql.NullInt32
		var username sql.NullString
		var rank sql.NullInt32

		err = rows.Scan(&uid, &username, &rank)
		checkErr(err)
		fmt.Println(uid)
		fmt.Println(username)
		fmt.Println(rank)
	}

	// delete
	fmt.Println("try to delete")
	stmt, err = db.Prepare("delete from customer where id=?")
	checkErr(err)

	res, err = stmt.Exec(1)
	checkErr(err)

	// query
	fmt.Println("try to select two")
	rows, err = db.Query("SELECT * FROM customer")
	checkErr(err)

	for rows.Next() {
		var uid int
		var username string
		var rank int

		err = rows.Scan(&uid, &username, &rank)
		checkErr(err)
		fmt.Println(uid)
		fmt.Println(username)
		fmt.Println(rank)
	}

	db.Close()

}

func test_tree() {
	fmt.Println("try to create plantree")
	var plan_tree iplan.PlanTree
	plan_tree.NodeNum = 5
	/*
	       0
	   1       2
	   3       4
	*/
	// 根结点0
	pn0 := &plan_tree.Nodes[0]
	pn0.Nodeid = 0
	pn0.Left = 1
	pn0.Right = 2
	pn0.Parent = -1
	pn0.Status = 0
	pn0.Locate = 0
	pn0.NodeType = 4
	pn0.TransferFlag = true
	pn0.Dest = 2
	pn0.Joint_cols = "id,customer_id"
	// 结点1
	pn1 := &plan_tree.Nodes[1]
	pn1.Nodeid = 1
	pn1.Left = 3
	pn1.Right = -1
	pn1.Parent = 0
	pn1.Status = 0
	pn1.Locate = 0
	pn1.NodeType = 2
	pn1.Where = "id > 2"
	// 结点2
	pn2 := &plan_tree.Nodes[2]
	pn2.Nodeid = 2
	pn2.Left = 4
	pn2.Right = -1
	pn2.Parent = 0
	pn2.Status = 0
	pn2.Locate = 0
	pn2.NodeType = 3
	pn2.Cols = "customer_id,quantity"
	// 结点3 data节点
	pn3 := &plan_tree.Nodes[3]
	pn3.Nodeid = 3
	pn3.Left = -1
	pn3.Right = -1
	pn3.Parent = 1
	pn3.Status = 1
	pn3.Locate = 0
	pn3.NodeType = 1
	pn3.TmpTable = "customer"
	// 结点4 data节点
	pn4 := &plan_tree.Nodes[4]
	pn4.Nodeid = 4
	pn4.Left = -1
	pn4.Right = -1
	pn4.Parent = 2
	pn4.Status = 1
	pn4.Locate = 0
	pn4.NodeType = 1
	pn4.TmpTable = "orders"
	var txn_id int64
	iexec.RunTree(txn_id)
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
