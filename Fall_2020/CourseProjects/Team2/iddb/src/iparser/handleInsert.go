package iparser

import (
	"fmt"
	"strconv"
	"strings"

	//"iqueryanalyzer"
	"github.com/xwb1989/sqlparser"
)

//map: (0,0)=1   <104000 && ="PRC"
var Publisherflag = [2][2]int{{0, 1}, {2, 3}}
var Bookflag = [3]int{0, 1, 1}
var Ordersflag = [2][2]int{{0, 1}, {2, 3}}

//return(1/2, [0][1], [0][1])
func HandleInsert(sql string) (int64, [2]string, [2]int64) {
	var TotalNum int64
	var outsql [2]string
	var siten [2]int64

	stmt, err := sqlparser.Parse(sql)
	if err != nil {
		// Do something with the err
		println("ERROR with parser!\n")
	}
	sel := stmt.(*sqlparser.Insert)

	rows := sqlparser.String(sel.Rows)
	rows = GetMidS(rows, "(", ")")
	value := strings.Split(rows, ",")

	tablename := sqlparser.String(sel.Table)
	tablename = strings.ToLower(tablename)
	switch tablename {
	case "customer":
		TotalNum = 2
		siten[0] = 0
		siten[1] = 1
		outsql[0] = "insert into customer_0 values(" + value[0] + "," + value[1] + ")"
		outsql[1] = "insert into customer_1 values(" + value[0] + "," + value[2] + ")"
	case "publisher":
		TotalNum = 1
		pidflag := -1
		nationflag := -1
		i := 0
		for i <= len(sel.Columns) {
			var col string
			col = sqlparser.String(sel.Columns[i])
			switch col {
			case "id":
				if Ifstr(value[i]) {
					fmt.Println("error id type")
					return TotalNum, outsql, siten
				}
				pid := v2int(value[i])
				if pid < 104000 {
					pidflag = 0
				} else {
					pidflag = 1
				}
			case "nation":
				if Ifstr(value[i]) == false {
					fmt.Println("error nation type")
					return TotalNum, outsql, siten
				}
				nation := v2str(value[i])
				if nation == "PRC" {
					nationflag = 0
				} else {
					nationflag = 0
				}
			}
			if (pidflag >= 0) && (nationflag >= 0) {
				break
			}
			i = i + 1
		}
		siten[0] = int64(Publisherflag[pidflag][nationflag])
		newtablename := tablename + "_" + strconv.Itoa(Publisherflag[pidflag][nationflag])
		outsql[0] = "insert into " + newtablename + " " + sqlparser.String(sel.Rows)
	case "book":
		TotalNum = 1
		bidflag := -1
		i := 0
		for i <= len(sel.Columns) {
			var col string
			col = sqlparser.String(sel.Columns[i])
			switch col {
			case "id":
				if Ifstr(value[i]) {
					fmt.Println("error id type")
					return TotalNum, outsql, siten
				}
				bid := v2int(value[i])
				if bid < 205000 {
					bidflag = 0
				}
				if (bid >= 205000) && (bid < 210000) {
					bidflag = 1
				}
				if bid >= 210000 {
					bidflag = 2
				}
			}
			if bidflag > 0 {
				break
			}
			i = i + 1
		}
		siten[0] = int64(Bookflag[bidflag])
		newtablename := tablename + "_" + strconv.Itoa(Bookflag[bidflag])
		outsql[0] = "insert into " + newtablename + " " + sqlparser.String(sel.Rows)
	case "orders":
		TotalNum = 1
		var ocidflag int
		var obidflag int
		i := 0
		for i <= len(sel.Columns) {
			var col string
			col = sqlparser.String(sel.Columns[i])
			switch col {
			case "customer_id":
				if Ifstr(value[i]) {
					fmt.Println("error id type")
					return TotalNum, outsql, siten
				}
				ocid := v2int(value[i])
				if ocid < 307000 {
					ocidflag = 0
				} else {
					ocidflag = 1
				}
			case "book_id":
				if Ifstr(value[i]) {
					fmt.Println("error id type")
					return TotalNum, outsql, siten
				}
				obid := v2int(value[i])
				if obid < 215000 {
					obidflag = 0
				} else {
					obidflag = 1
				}
			}
			if (ocidflag >= 0) && (obidflag >= 0) {
				break
			}
			i = i + 1
		}
		siten[0] = int64(Ordersflag[ocidflag][obidflag])
		newtablename := tablename + "_" + strconv.Itoa(Ordersflag[ocidflag][obidflag])
		outsql[0] = "insert into " + newtablename + " " + sqlparser.String(sel.Rows)
	}
	return TotalNum, outsql, siten
}

func GetMidS(s string, sep1 string, sep2 string) string {
	c1 := strings.Index(s, sep1) + 1
	c2 := strings.LastIndex(s, sep2)
	s = string([]byte(s)[c1:c2])
	return s
}

func Ifstr(s string) bool {
	if strings.Contains(s, "'") {
		return true
	} else {
		return false
	}
}

func v2int(s string) int {
	s = strings.TrimSpace(s)
	num, err := strconv.ParseInt(s, 10, 64)
	if err != nil {
		fmt.Println(err)
	}
	return int(num)
}

func v2str(s string) string {
	s = GetMidS(s, "'", "'")
	return s
}
