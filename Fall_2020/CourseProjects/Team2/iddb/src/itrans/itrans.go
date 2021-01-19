package itrans

import (
	"database/sql"
	"iutilities"

	_ "github.com/go-sql-driver/mysql"
)

var site int64
var mysql_user string
var mysql_passwd string
var mysql_db string
var mysql_ip_port string
var db *sql.DB
var err error

func Init() {
	// TODO:set site id
	site = iutilities.GetMe().NodeId
	// site = 1
	mysql_user = iutilities.Mysql.Mysql_user
	mysql_passwd = iutilities.Mysql.Mysql_passwd
	mysql_db = iutilities.Mysql.Mysql_db
	mysql_ip_port = iutilities.Mysql.Mysql_ip_port
	mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	println(mysql)
	db, err = sql.Open("mysql", mysql)
	if err != nil {
		println("could not open:", err)
		iutilities.CheckErr(err)
	}

}

func ExecuteCreateStmt(stmt string) int64 {
	Init()
	// mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
	// db, err := sql.Open("mysql", mysql)
	// if err != nil {
	// 	println("could not open: %v", err)
	// }
	println("stmt length is :", len(stmt))
	if len(stmt) > 1000 {
		println(stmt[0:200])
	} else {
		println(stmt)
	}
	// println(stmt)

	// println(err)
	stmts, err := db.Prepare(stmt)
	if err != nil {
		println("could not prepare:", err)
		iutilities.CheckErr(err)
	}
	res, err := stmts.Exec()
	if err != nil {
		println("could not exec:", err)
	}
	ins_id, err := res.LastInsertId()
	row_aff, err := res.RowsAffected()

	println("res: ", ins_id, row_aff)
	println(err)
	return 0
}
