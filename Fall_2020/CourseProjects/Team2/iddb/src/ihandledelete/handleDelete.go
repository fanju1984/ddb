package ihandledelete

import (
	"fmt"
	"iexec"
	"imeta"
	"iparser"
	"iqueryanalyzer"
	"iqueryoptimizer"
	"irpccall"
	"iutilities"
	"strconv"
	"strings"

	"github.com/xwb1989/sqlparser"
)

var txnID int64 = 55598

func HandleDelete(sql string) (int64, [8]string, [8]int64) {
	var TotalNum int64
	var outsql [8]string
	var siten [8]int64

	stmt, err := sqlparser.Parse(sql)
	if err != nil {
		// Do something with the err
		println("ERROR with parser!\n")
	}
	sel := stmt.(*sqlparser.Delete)

	tablename := sqlparser.String(sel.TableExprs)
	tablename = strings.ToLower(tablename)
	fmt.Println(tablename)
	switch tablename {
	case "orders":
		TotalNum = 4
		i := 0
		for i < int(TotalNum) {
			siten[i] = int64(i)
			strwhere := sqlparser.String(sel.Where)
			strwhere = strings.Replace(strwhere, "customer_id", "ocid", -1)
			strwhere = strings.Replace(strwhere, "book_id", "obid", -1)
			outsql[i] = "delete from orders_" + strconv.Itoa(i) + strwhere
			i = i + 1
		}
	case "book":
		TotalNum = 3
		i := 0
		for i < int(TotalNum) {
			siten[i] = int64(i)
			strwhere := sqlparser.String(sel.Where)
			strwhere = strings.Replace(strwhere, "id", "bid", -1)
			strwhere = strings.Replace(strwhere, "publisher_id", "bpid", -1)
			outsql[i] = "delete from book_" + strconv.Itoa(i) + strwhere
			i = i + 1
		}
	case "publisher":
		TotalNum = 4
		i := 0
		for i < int(TotalNum) {
			siten[i] = int64(i)
			strwhere := sqlparser.String(sel.Where)
			strwhere = strings.Replace(strwhere, "id", "pid", -1)
			strwhere = strings.Replace(strwhere, "name", "pname", -1)
			outsql[i] = "delete from publisher_" + strconv.Itoa(i) + strwhere
			i = i + 1
		}
	case "customer":
		//step1 find cid
		strwhere := sqlparser.String(sel.Where)
		strwhere = strings.Replace(strwhere, "id", "cid", -1)
		strwhere = strings.Replace(strwhere, "name", "cname", -1)
		sqlstmt := "select cid from customer" + strwhere

		//txnID needs to be unique!
		txnID++
		plantree := iparser.Parse(sqlstmt, txnID)
		plantree = iqueryanalyzer.Analyze(plantree)
		plantree = iqueryoptimizer.Optimize(plantree)

		ipaddr0 := iutilities.Peers[0].IP + ":" + iutilities.Peers[0].Call

		ipaddr1 := iutilities.Peers[1].IP + ":" + iutilities.Peers[1].Call

		plantree.Print()

		imeta.Connect_etcd()
		println("start imeta")

		err = imeta.Build_Txn(txnID)
		if err != nil {
			iutilities.CheckErr(err)
		}

		println("imeta build txn ok")

		err = imeta.Set_Tree(txnID, plantree)
		if err != nil {
			iutilities.CheckErr(err)
		}
		println("imeta set tree ok")

		println("end imeta")

		iutilities.Waitgroup.Add(1)
		go irpccall.RunCallClient(ipaddr0, txnID)

		iutilities.Waitgroup.Add(1)
		go irpccall.RunCallClient(ipaddr1, txnID)

		iutilities.Waitgroup.Wait()

		plantree, err = imeta.Get_Tree(txnID)

		if err != nil {
			iutilities.CheckErr(err)
		}

		res := iexec.GetResult(plantree, txnID)

		println(res)

		//res :=[] int {11,22,33 }

		//step2 delete by cid
		//TotalNum = 4
		if len(res) > 4 {
			TotalNum = 8
		} else {
			TotalNum = int64(2 * len(res))
		}
		j := 0
		i := 0
		for i < int(TotalNum) {
			//delete from customer_0123 where cid=res[0]
			siten[i] = int64(0)
			outsql[i] = "delete from customer_0 where cid=" + strconv.Itoa(res[j])
			i = i + 1
			siten[i] = int64(1)
			outsql[i] = "delete from customer_1 where cid=" + strconv.Itoa(res[j])
			i = i + 1
			j = j + 1
		}
	}
	return TotalNum, outsql, siten

}
