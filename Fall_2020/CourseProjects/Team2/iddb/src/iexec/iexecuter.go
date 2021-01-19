package iexec

import (
	"database/sql"
	"fmt"
	"imeta"
	"iplan"
	"irpctran"
	"iutilities"
	"reflect"
	"strconv"
	"strings"
	"time"

	_ "github.com/go-sql-driver/mysql"
)

var site int64
var mysql_user string
var mysql_passwd string
var mysql_db string
var mysql_ip_port string
var db *sql.DB
var err error

func RunExecuter(txn_id int64) int64 {
	// get the plan through txn_id
	//
	RunTree(txn_id)
	println("total transmission =", iutilities.Total_trans, "B")
	println("executed successfully")

	return 0
}

// func ExecuteCreateStmt(stmt string) int64 {
// 	println(stmt)
// 	return 0
// }

func ExecuteRemoteCreateStmt(address string, create_sql string) {
	var table irpctran.Table
	table.Createstmt = create_sql
	iutilities.Total_trans += int64(len(create_sql))

	irpctran.RunTranClient(address, table)
	println("total_trans =", iutilities.Total_trans)
}

func Init() {
	// TODO:set site id
	site = iutilities.Me.NodeId
	// site = 1
	mysql_user = iutilities.Mysql.Mysql_user
	mysql_passwd = iutilities.Mysql.Mysql_passwd
	mysql_db = iutilities.Mysql.Mysql_db
	mysql_ip_port = iutilities.Mysql.Mysql_ip_port
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err = sql.Open("mysql", mysql)
	iutilities.CheckErr(err)
	// db.Close()
}

func RunTree(txn_id int64) int64 {
	Init()
	println("iexecuter init ")
	var plan_tree iplan.PlanTree
	cnt := 0
	for {
		// TODO:获取etcd tree
		cnt++

		plan_tree, err = imeta.Get_Tree(txn_id)
		// 检测树是否完全执行完毕
		if TreeIsComplete(plan_tree) {
			break
		}
		// 从tree中找可用代码
		// fmt.Println(plan_tree)
		plan_tree.Print()
		execute_id := FindOneNode(plan_tree, plan_tree.Root)
		if execute_id == -1 {
			if cnt == 100 {
				cnt = 0
				println("txn_id:", txn_id, " busy waiting")
			}
			continue
		}

		fmt.Println(execute_id)

		var pn *iplan.PlanTreeNode
		pn = &plan_tree.Nodes[execute_id]
		// 执行某个节点
		print("executed node")
		println(pn.Nodeid)
		print("current node state ")
		ExecuteOneNode(pn, plan_tree, txn_id)

		var current_node iplan.PlanTreeNode
		current_node = plan_tree.Nodes[execute_id]
		fmt.Println(current_node)
		// TODO:更新etcd状态
		imeta.Set_Node(txn_id, current_node)
		time.Sleep(time.Duration(10) * time.Millisecond)
	}
	println("executed successfully")
	return 0
}

func TreeIsComplete(plan_tree iplan.PlanTree) bool {
	var root_node iplan.PlanTreeNode
	root_node = plan_tree.Nodes[plan_tree.Root]
	if root_node.Status == 1 {
		return true
	} else {
		return false
	}
}

func FindOneNode(plan_tree iplan.PlanTree, node_id int64) int64 {
	Init()
	// fmt.Println("go into node")
	// fmt.Println(node_id)
	var can_execute_id int64
	can_execute_id = -1
	// 判断当前节点的状态，如果是ok，则返回-1
	var current_node iplan.PlanTreeNode
	current_node = plan_tree.Nodes[node_id]
	// fmt.Println("current_node is :")
	// fmt.Println(current_node)
	if current_node.Status == 1 {
		// fmt.Println("current node has done")
		return can_execute_id
	}
	// 否则判断两个子节点的状态，如果都是ok，则返回当前节点id
	var left_node iplan.PlanTreeNode
	var right_node iplan.PlanTreeNode
	var can_execute bool
	can_execute = true
	if current_node.Left != -1 {
		left_node = plan_tree.Nodes[current_node.Left]
		if left_node.Status == 0 {
			can_execute = false
		}
	}
	if current_node.Right != -1 {
		right_node = plan_tree.Nodes[current_node.Right]
		if right_node.Status == 0 {
			can_execute = false
		}
	}
	if can_execute && current_node.Locate == site {
		// fmt.Println("current node can execute")
		can_execute_id = node_id
		return can_execute_id
	}
	if current_node.Left != -1 {
		can_execute_id = FindOneNode(plan_tree, current_node.Left)
	}
	if can_execute_id != -1 {
		return can_execute_id
	}
	if current_node.Right != -1 {
		can_execute_id = FindOneNode(plan_tree, current_node.Right)
	}
	if can_execute_id != -1 {
		return can_execute_id
	}
	return -1
}

func ExecuteOneNode(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
	Init()
	switch {
	case plan_node.NodeType == 1 /*Table*/ :
		break
	case plan_node.NodeType == 2 /*Select or Filter*/ :
		ExecuteFilter(plan_node, plan_tree, txn_id)
		break
	case plan_node.NodeType == 3 /*projuection*/ :
		ExecuteProjection(plan_node, plan_tree, txn_id)
		break
	case plan_node.NodeType == 4 /*join*/ :
		ExecuteJoin(plan_node, plan_tree, txn_id)
		break
	case plan_node.NodeType == 5 /*union*/ :
		ExecuteUnion(plan_node, plan_tree, txn_id)
		break
	}
	ExecuteTransmission(plan_node)
}

func CleanTmpTable(plan_node_id int64, plan_tree iplan.PlanTree) {
	nodeType := plan_tree.Nodes[plan_node_id].NodeType
	Locate := plan_tree.Nodes[plan_node_id].Locate
	if nodeType != 1 || Locate != site {
		tablename := plan_tree.Nodes[plan_node_id].TmpTable
		// TODO: assert(plan_node.Right = -1)
		query := "drop table if exists " + tablename
		println(query)
		stmt, err := db.Prepare(query)
		res, err := stmt.Exec()
		iutilities.CheckErr(err)
		println(res)
	}
}

func ExecuteFilter(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	// TODO: assert(plan_node.Right = -1)
	var query string
	tablename := plan_tree.Nodes[plan_node.Left].TmpTable
	if strings.EqualFold(plan_node.Cols, "") {
		query = "create table " + plan_node.TmpTable + " select * from " + tablename + " " + plan_node.Where
	} else {
		query = "create table " + plan_node.TmpTable + " select " + plan_node.Cols + " from " + tablename + " " + plan_node.Where
	}

	println(query)
	stmt, err := db.Prepare(query)
	res, err := stmt.Exec()
	iutilities.CheckErr(err)
	println(res)
	// plan_node.TmpTable = "tmp_table_" + strconv.FormatInt(txn_id, 10) + "_" + strconv.FormatInt(plan_node.Nodeid, 10)
	AddIndex(plan_node)
	CleanTmpTable(plan_node.Left, plan_tree)
	if !plan_node.TransferFlag {
		plan_node.Status = 1
	}
	// db.Close()
}

func ExecuteProjection(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	// TODO: assert(plan_node.Right = -1)

	tablename := plan_tree.Nodes[plan_node.Left].TmpTable
	query := "create table " + plan_node.TmpTable + " select " + plan_node.Cols + " from " + tablename
	println(query)

	stmt, err := db.Prepare(query)
	res, err := stmt.Exec()
	iutilities.CheckErr(err)
	println(res)
	// plan_node.TmpTable = "tmp_table_" + strconv.FormatInt(txn_id, 10) + "_" + strconv.FormatInt(plan_node.Nodeid, 10)
	AddIndex(plan_node)
	CleanTmpTable(plan_node.Left, plan_tree)
	if !plan_node.TransferFlag {
		plan_node.Status = 1
	}
	// db.Close()
}

func ExecuteJoin(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	// TODO: assert(plan_node.Right != -1)

	tablename1 := plan_tree.Nodes[plan_node.Left].TmpTable
	tablename2 := plan_tree.Nodes[plan_node.Right].TmpTable
	var query string
	if plan_node.Joint_type == 0 {
		query = "create table " + plan_node.TmpTable + " select * from " + tablename1 + "," + tablename2 + ";"
		println(query)
	} else if plan_node.Joint_type == 1 {
		query = "create table " + plan_node.TmpTable + " select * from " + tablename1 + "," + tablename2 + " " + plan_node.Where + ";"
		println(query)
	} else {
		query = "create table " + plan_node.TmpTable + " select * from " + tablename1 + " natural join " + tablename2 + " " + plan_node.Where + ";"
		println(query)
	}

	stmt, err := db.Prepare(query)
	res, err := stmt.Exec()
	iutilities.CheckErr(err)
	println(res)
	// plan_node.TmpTable = "tmp_table_" + strconv.FormatInt(txn_id, 10) + "_" + strconv.FormatInt(plan_node.Nodeid, 10)
	AddIndex(plan_node)
	CleanTmpTable(plan_node.Left, plan_tree)
	CleanTmpTable(plan_node.Right, plan_tree)
	if !plan_node.TransferFlag {
		plan_node.Status = 1
	}
	// db.Close()
}

func ExecuteUnion(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	// TODO: assert(plan_node.Right != -1)

	tablename1 := plan_tree.Nodes[plan_node.Left].TmpTable
	tablename2 := plan_tree.Nodes[plan_node.Right].TmpTable
	query := "create table " + plan_node.TmpTable + " select * from " + tablename1 + " union all" + " select * from " + tablename2 + ";"
	println(query)

	stmt, err := db.Prepare(query)
	res, err := stmt.Exec()
	iutilities.CheckErr(err)
	println(res)
	// plan_node.TmpTable = "tmp_table_" + strconv.FormatInt(txn_id, 10) + "_" + strconv.FormatInt(plan_node.Nodeid, 10)
	AddIndex(plan_node)
	CleanTmpTable(plan_node.Left, plan_tree)
	CleanTmpTable(plan_node.Right, plan_tree)
	if !plan_node.TransferFlag {
		plan_node.Status = 1
	}
	// db.Close()
}

func AddIndex(plan_node *iplan.PlanTreeNode) {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	iutilities.CheckErr(err)

	index_querys, has_index := generateAddIndexQuery(plan_node)

	if has_index {
		for _, query := range index_querys {
			stmt, err := db.Prepare(query)
			println("create index stmt:", query)
			res, err := stmt.Exec()
			iutilities.CheckErr(err)
			println(res)
		}
	}
	// db.Close()
}

func Strval(value interface{}) string {
	var key string
	if value == nil {
		return key
	}

	switch value.(type) {
	case float64:
		ft := value.(float64)
		key = strconv.FormatFloat(ft, 'f', -1, 64)
	case float32:
		ft := value.(float32)
		key = strconv.FormatFloat(float64(ft), 'f', -1, 64)
	case int:
		it := value.(int)
		key = strconv.Itoa(it)
	case uint:
		it := value.(uint)
		key = strconv.Itoa(int(it))
	case int8:
		it := value.(int8)
		key = strconv.Itoa(int(it))
	case uint8:
		it := value.(uint8)
		key = strconv.Itoa(int(it))
	case int16:
		it := value.(int16)
		key = strconv.Itoa(int(it))
	case uint16:
		it := value.(uint16)
		key = strconv.Itoa(int(it))
	case int32:
		it := value.(int32)
		key = strconv.Itoa(int(it))
	case uint32:
		it := value.(uint32)
		key = strconv.Itoa(int(it))
	case int64:
		it := value.(int64)
		key = strconv.FormatInt(it, 10)
	case uint64:
		it := value.(uint64)
		key = strconv.FormatUint(it, 10)
	case string:
		key = "'" + value.(string) + "'"
	case []byte:
		key = "'" + string(value.([]byte)) + "'"
	case sql.RawBytes:
		key = "'" + string(value.(sql.RawBytes)) + "'"
	case sql.NullBool:
		boolnull := value.(sql.NullBool)
		if boolnull.Valid {
			key = strconv.FormatBool(boolnull.Bool)
		} else {
			key = "NULL"
		}
	case sql.NullString:
		stringnull := value.(sql.NullString)
		if stringnull.Valid {
			key = "'" + stringnull.String + "'"
		} else {
			key = "NULL"
		}
	case sql.NullFloat64:
		float64null := value.(sql.NullFloat64)
		if float64null.Valid {
			key = strconv.FormatFloat(float64null.Float64, 'f', -1, 64)
		} else {
			key = "NULL"
		}
	case sql.NullInt32:
		int32null := value.(sql.NullInt32)
		if int32null.Valid {
			key = strconv.Itoa(int(int32null.Int32))
		} else {
			key = "NULL"
		}
	case sql.NullInt64:
		int64null := value.(sql.NullInt64)
		if int64null.Valid {
			key = strconv.FormatInt(int64null.Int64, 10)
		} else {
			key = "NULL"
		}

	default:
		// newValue, _ := json.Marshal(value)
		// key = string(newValue)
	}

	return key
}

func generateCreateQuery(plan_node *iplan.PlanTreeNode) string {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	iutilities.CheckErr(err)
	// create table
	query := "show create table " + plan_node.TmpTable
	// println(query)
	rows, err := db.Query(query)
	rows.Next()
	var table_name sql.NullString
	var create_sql sql.NullString
	err = rows.Scan(&table_name, &create_sql)
	iutilities.CheckErr(err)
	fmt.Println(table_name.String)
	// fmt.Println(create_sql.String + ";")
	// db.Close()
	return create_sql.String + ";"
}

func generateInsertQuery(plan_node *iplan.PlanTreeNode) ([]string, bool) {
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	db, err := sql.Open("mysql", mysql)
	mySlice := make([]string, 0)
	insert_query := "insert into " + plan_node.TmpTable + " values "
	query := "select * from " + plan_node.TmpTable
	// println(query)
	rows, err := db.Query(query)

	tt, err := rows.ColumnTypes()

	iutilities.CheckErr(err)

	types := make([]reflect.Type, len(tt))
	for i, tp := range tt {
		// ScanType
		scanType := tp.ScanType()
		types[i] = scanType
	}
	// fmt.Println(" ")
	values := make([]interface{}, len(tt))
	for i := range values {
		values[i] = reflect.New(types[i]).Interface()
	}
	i := 0
	for rows.Next() {
		if i%1000 == 0 && i != 0 {
			insert_query = insert_query + ";"
			mySlice = append(mySlice, insert_query)
			insert_query = "insert into " + plan_node.TmpTable + " values "
		} else if i != 0 {
			insert_query = insert_query + ", "
		}
		err = rows.Scan(values...)
		iutilities.CheckErr(err)
		insert_query = insert_query + "("
		for j := range values {
			if j != 0 {
				insert_query = insert_query + ", "
			}
			value := reflect.ValueOf(values[j]).Elem().Interface()
			insert_query = insert_query + Strval(value)
			// fmt.Print(Strval(value))
			// fmt.Print(" ")
		}
		insert_query = insert_query + ")"
		// fmt.Println(" ")
		i++
	}
	insert_query = insert_query + ";"
	mySlice = append(mySlice, insert_query)
	// db.Close()
	if i == 0 {
		return mySlice, false
	} else {
		return mySlice, true
	}

}

func generateAddIndexQuery(plan_node *iplan.PlanTreeNode) ([]string, bool) {
	mySlice := make([]string, 0)
	has_index := false
	cols := strings.Split(plan_node.Rel_cols, ",")
	for _, col := range cols {
		if strings.EqualFold(col, "pid") {
			query := "create index idx_pid_" + plan_node.TmpTable + " on " + plan_node.TmpTable + "(pid)"
			mySlice = append(mySlice, query)
			has_index = true
		} else if strings.EqualFold(col, "bid") {
			query := "create index idx_bid_" + plan_node.TmpTable + " on " + plan_node.TmpTable + "(bid)"
			mySlice = append(mySlice, query)
			has_index = true
		} else if strings.EqualFold(col, "cid") {
			query := "create index idx_cid_" + plan_node.TmpTable + " on " + plan_node.TmpTable + "(cid)"
			mySlice = append(mySlice, query)
			has_index = true
		} else if strings.EqualFold(col, "ocid") {
			query := "create index idx_ocid_" + plan_node.TmpTable + " on " + plan_node.TmpTable + "(ocid)"
			mySlice = append(mySlice, query)
			has_index = true
		} else if strings.EqualFold(col, "obid") {
			query := "create index idx_obid_" + plan_node.TmpTable + " on " + plan_node.TmpTable + "(obid)"
			mySlice = append(mySlice, query)
			has_index = true
		}
	}
	return mySlice, has_index
}

func getAddress(plan_node *iplan.PlanTreeNode) string {
	dest := iutilities.Peers[plan_node.Dest]
	address := dest.IP + ":" + dest.Tran
	return address
}

func ExecuteTransmission(plan_node *iplan.PlanTreeNode) {
	if plan_node.TransferFlag {
		address := getAddress(plan_node)
		fmt.Println(address)
		create_sql := generateCreateQuery(plan_node)
		fmt.Println(create_sql)
		ExecuteRemoteCreateStmt(address, create_sql)
		insert_query, issuccess := generateInsertQuery(plan_node)

		println("query length is: ", len(insert_query))

		// index_querys, has_index := generateAddIndexQuery(plan_node)

		// if has_index {
		// 	for _, query := range index_querys {
		// 		ExecuteRemoteCreateStmt(address, query)
		// 	}
		// }
		if issuccess {
			for _, query := range insert_query {
				ExecuteRemoteCreateStmt(address, query)
			}
		}
		plan_node.Status = 1
	}
}

func PrintResult(plan_tree iplan.PlanTree, txnID int64) {
	Init()
	if TreeIsComplete(plan_tree) != true {
		println("txn ", txnID, "not finished!")
	} else {
		mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
		db, err := sql.Open("mysql", mysql)

		plan_node := plan_tree.Nodes[plan_tree.Root]

		query := "select * from " + plan_node.TmpTable
		println(query)
		rows, err := db.Query(query)
		iutilities.CheckErr(err)
		tt, err := rows.ColumnTypes()
		iutilities.CheckErr(err)

		types := make([]reflect.Type, len(tt))
		for i, tp := range tt {
			// ScanType
			scanType := tp.ScanType()
			types[i] = scanType
		}
		// fmt.Println(" ")
		values := make([]interface{}, len(tt))
		for i := range values {
			values[i] = reflect.New(types[i]).Interface()
		}
		i := 0
		for rows.Next() {
			// todo: 只插入前100条，之后需要修改
			if i > 30 {
				break
			}
			// todo: 只插入前100条，之后需要修改
			err = rows.Scan(values...)
			iutilities.CheckErr(err)
			fmt.Print("|")
			for j := range values {

				value := reflect.ValueOf(values[j]).Elem().Interface()
				fmt.Print(Strval(value))
				fmt.Print("|")
			}
			fmt.Println(" ")
			i++
		}

		var count int64
		err = db.QueryRow("select count(*) from " + plan_node.TmpTable).Scan(&count)
		fmt.Print("total count:")
		fmt.Println(count)

		// db.Close()
	}
}

func GetResult(plan_tree iplan.PlanTree, txnID int64) []int {
	mySlice := make([]int, 0)
	Init()
	if TreeIsComplete(plan_tree) != true {
		println("txn ", txnID, "not finished!")
	} else {
		mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
		db, err := sql.Open("mysql", mysql)

		plan_node := plan_tree.Nodes[plan_tree.Root]

		query := "select * from " + plan_node.TmpTable
		// println(query)
		rows, err := db.Query(query)
		iutilities.CheckErr(err)

		i := 0
		for rows.Next() {
			var id int
			err = rows.Scan(&id)
			mySlice = append(mySlice, id)
			i++
		}
		// db.Close()
	}
	return mySlice
}
