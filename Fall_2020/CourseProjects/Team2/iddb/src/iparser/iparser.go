package iparser

import (
	"github.com/xwb1989/sqlparser"
	"iplan"
)

//TransactionID record the sql id which is processing now
var TransactionID int64 = 0

//Parse will parse a sql string into an ast, and build a basic plantree.
func Parse(sql string, TxnID int64) (planTree iplan.PlanTree) {
	// sql := "SELECT a FROM t1,t2 WHERE t1.a=t2.a"
	// sql := "select * from x_order where userId = 1 order by id desc limit 10,1;"
	TransactionID = TxnID
	stmt, err := sqlparser.Parse(sql)
	if err != nil {
		// Do something with the err
		println("ERROR with parser!\n")
	}

	// Otherwise do something with stmt
	switch stmt.(type) {
	case *sqlparser.Select:
		planTree = buildSelect(stmt.(*sqlparser.Select))

	case *sqlparser.Update:
		// return handleUpdate(stmt.(*sqlparser.Update))
	case *sqlparser.Insert:
		// return handleInsert(stmt.(*sqlparser.Insert))
		buildInsert(stmt.(*sqlparser.Insert))
	case *sqlparser.Delete:
		// return handleDelete(stmt.(*sqlparser.Delete))
	}
	return planTree
}
