package main

import (
	"ihandledelete"
	"iparser"
	"iqueryanalyzer"
	"time"

	// "iqueryanalyzer"
	"iqueryoptimizer"
	// "imeta"
	// "irpc"
	"iexec"
	// "context"
	// "iexecuter"
	// "log"
	// "net"

	"os"

	"imeta"
	"iplan"
	"irpccall"
	"iutilities"
	"strconv"

	// "iparser"
	"fmt"
	"runtime"
	"strings"
)

/*
iddb client设计思路
1.默认SERVER已经启动
2.循环接收用户输入，提供类似mysql的界面
3.初始化，知道自己和peers的信息
4.生成生成txnid(全局唯一，严格递增)
*/

var (
	txnID        int64
	err          error
	ipaddr       string
	plantree     iplan.PlanTree
	queries      [15]string
	test_insert_ = 1
	test_delete_ = 1
	test_select_ = 0
	test_flag_   = 0
)

func main() {
	txnID = 576
	iutilities.LoadAllConfig()
	runtime.GOMAXPROCS(8)

	for i, v := range os.Args {
		if i == 1 {
			test_insert_, _ = strconv.Atoi(v)
		}

		if i == 2 {
			test_delete_, _ = strconv.Atoi(v)
		}

		if i == 3 {
			test_select_, _ = strconv.Atoi(v)
		}

		if i == 4 {
			test_flag_, _ = strconv.Atoi(v)
		}

	}

	println(test_insert_, test_delete_, test_select_)

	if test_insert_ == 1 {
		test_insert()
	}

	if test_delete_ == 1 {
		test_delete()
	}

	if test_select_ != 1 {
		return
	}

	var sqlstmt string
	queries[0] = `
	select *
	from customer`

	queries[1] = `
	select publisher.name
	from publisher`

	queries[2] = `
	select book.title
	from book
	where copies>5000`

	queries[3] = `
	select orders.customer_id,quantity
	from orders
	where quantity<8`

	queries[4] = `
	select book.title, book.copies, publisher.name, publisher.nation
	from book, publisher
	where book.publisher_id = publisher.id
	and publisher.nation='USA'
	and book.copies>1000`

	queries[5] = `
	select customer.name, orders.quantity
	from customer,orders
	where customer.id=orders.customer_id`

	queries[6] = `
	select customer.name, customer.rank, orders.quantity
	from customer,orders
	where customer.id=orders.customer_id
	and customer.rank=1`

	queries[7] = `
	select customer.name, orders.quantity, book.title
	from customer,orders,book
	where customer.id=orders.customer_id
	and book.id=orders.book_id
	and customer.rank=1
	and book.copies>5000`

	queries[8] = `
	select customer.name, book.title, publisher.name, orders.quantity
	from customer, book, publisher, orders
	where customer.id=orders.customer_id
	and book.id=orders.book_id
	and book.publisher_id=publisher.id
	and book.id>220000
	and publisher.nation='USA'
	and orders.quantity>1`

	queries[9] = `
	select customer.name, book.title, publisher.name, orders.quantity
	from customer, book, publisher, orders
	where customer.id=orders.customer_id
	and book.id=orders.book_id
	and book.publisher_id=publisher.id
	and customer.id>308000
	and book.copies>100
	and orders.quantity>1
	and publisher.nation='PRC'`

	queries[10] = `
	select customer.name, book.title, publisher.name, orders.quantity 
	from customer, book, publisher, orders 
	where customer.id=orders.customer_id 
	and book.id=orders.book_id 
	and book.publisher_id=publisher.id 
	and book.id > 207000 
	and book.id < 213000 
	and book.copies>100 
	and orders.quantity>1 
	and publisher.nation='PRC'
	`

	println("please enter TxnId: ")
	txnID, err = strconv.ParseInt(scanLine(), 10, 64)
	if err != nil {
		iutilities.CheckErr(err)
	} else {
		println("txnID=", txnID)
	}

	for qid := 0; qid < 11; qid++ {

		if test_flag_ == 0 {
			if qid != 0 {
				continue
			}
		}
		if test_flag_ == 1 {
			if qid != 1 {
				continue
			}
		}
		if test_flag_ == 2 {
			if qid != 2 {
				continue
			}
		}
		if test_flag_ == 3 {
			if qid != 3 {
				continue
			}
		}
		if test_flag_ == 4 {
			if qid != 4 {
				continue
			}
		}
		if test_flag_ == 5 {
			if qid != 5 {
				continue
			}
		}
		if test_flag_ == 6 {
			if qid != 6 {
				continue
			}
		}
		if test_flag_ == 7 {
			if qid != 7 {
				continue
			}
		}
		if test_flag_ == 8 {
			if qid != 8 {
				continue
			}
		}

		if test_flag_ == 9 {
			if qid != 9 {
				continue
			}
		}

		if test_flag_ == 10 {
			if qid != 10 {
				continue
			}
		}
		println("press any key to continue")
		_ = scanLine()

		now := time.Now()
		sqlstmt = queries[qid]

		println(sqlstmt)
		if strings.EqualFold(sqlstmt, "q") {
			break
		}

		plantree := iparser.Parse(sqlstmt, txnID)
		plantree = iqueryanalyzer.Analyze(plantree)
		plantree = iqueryoptimizer.Optimize(plantree)

		relsites := iqueryoptimizer.GetRelSites(plantree)
		for i := 0; i < 4; i++ {
			if relsites[i] == 1 {
				println("Relative Site", i)
			}
		}

		// fmt.Println("plantree is ", plantree)

		plantree.Print()

		imeta.Connect_etcd()
		println("start imeta")

		err = imeta.Build_Txn(txnID)
		if err != nil {
			iutilities.CheckErr(err)
			return
		}

		println("imeta build txn ok")

		err = imeta.Set_Tree(txnID, plantree)
		if err != nil {
			iutilities.CheckErr(err)
			return
		}
		println("imeta set tree ok")

		println("end imeta")
		for i, node := range iutilities.Peers {
			if relsites[i] == 0 {
				continue
			}
			ipaddr = node.IP + ":" + node.Call
			println("call node to work ", node.NodeId)
			iutilities.Waitgroup.Add(1)
			go irpccall.RunCallClient(ipaddr, txnID)
		}

		iutilities.Waitgroup.Wait()

		plantree, err = imeta.Get_Tree(txnID)

		if err != nil {
			iutilities.CheckErr(err)
			return
		}

		iexec.PrintResult(plantree, txnID)

		println("total time cost: ")
		println(time.Since(now).Milliseconds(), " ms")

		println("txn", txnID, "end!")

		txnID += 1

	}

	return
}

func scanLine() string {
	var c byte
	var err error
	var b []byte
	for err == nil {
		_, err = fmt.Scanf("%c", &c)

		if c != '\n' {
			b = append(b, c)
		} else {
			break
		}
	}

	return string(b)
}

func test_insert() {
	var ins_stmts [5]string
	ins_stmts[0] = `
	insert into customer(id, name, rank) values(300001, 'Xiaoming', 1)`

	ins_stmts[1] = `
	insert into publisher(id, name, nation) values(104001,'High Education Press', 'PRC')`

	ins_stmts[2] = `
	insert into customer(id, name, rank) values(300002,'Xiaohong', 1)`

	ins_stmts[3] = `
	insert into book (id, title, authors, publisher_id, copies) values(205001, 'DDB', 'Oszu', 104001, 100)`

	ins_stmts[4] = `
	insert into orders (customer_id, book_id, quantity) values(300001, 205001,5)`

	for i, ins_stmt := range ins_stmts {
		println(i, ins_stmt)

		N, sqls, siteid := iparser.HandleInsert(ins_stmt)
		j := 0
		for int64(j) < N {
			println("insert_stmt", j, siteid[j], sqls[j])
			println("press any key to continue")
			_ = scanLine()
			RunRemoteStmt(siteid[j], sqls[j])
			j++
		}

	}
	return
}

func test_delete() {
	var del_stmts [5]string
	del_stmts[0] = `
	delete from orders`

	del_stmts[1] = `
	delete from book where copies = 100`

	del_stmts[2] = `
	delete from publisher where nation = 'PRC'`

	del_stmts[3] = `
	delete from customer where name='Xiaohong' AND rank=1`

	del_stmts[4] = `
	delete from customer where rank = 1`

	for i, del_stmt := range del_stmts {
		println(i, del_stmt)
		N, sqls, siteid := ihandledelete.HandleDelete(del_stmt)
		j := 0
		for int64(j) < N {
			println("delete_stmt", j, siteid[j], sqls[j])
			println("press any key to continue")
			_ = scanLine()
			RunRemoteStmt(siteid[j], sqls[j])
			j = j + 1
		}

	}
	return
}

func RunRemoteStmt(siteid int64, stmt string) {
	node := iutilities.Peers[siteid]
	ipaddr = node.IP + ":" + node.Tran
	iexec.ExecuteRemoteCreateStmt(ipaddr, stmt)
}
