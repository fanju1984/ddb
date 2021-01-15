package main

import (
	"irpctran"
	"iutilities"
)

/*
type Table struct {
	Operation string    //insert into TableName (Col1, Col2, Col3)
	Rowlength int64     //10
	Record    []string  //(Val1, Val2, Val3)
}
*/
func main() {
	iutilities.Peers = iutilities.GetPeers()
	testnodeid := 0
	ip := iutilities.Peers[testnodeid].IP
	port := iutilities.Peers[testnodeid].Tran
	address := ip + ":" + port
	println(address)
	// address := "localhost:50053"
	var table irpctran.Table
	table.Createstmt = "Create Table PUBLISHER1 (ID int, NATION varchar(255) );"
	irpctran.RunTranClient(address, table)
}
