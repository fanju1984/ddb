/*
** 这里面所有的函数用于获取存储再ETCD得元信息, 包括分片数量，模式，表得属性与相应类型等。
*/
package gddGet

import (
	"fmt"
	"gDB/storage"
	"github.com/coreos/etcd/clientv3"
	"github.com/juju/errors"
	"strconv"
	"strings"
	"unsafe"
)

var Client *clientv3.Client

func Init(c *clientv3.Client) {
	Client = c
}

// Given table name return the all fragment such as Student.1,Student.2
func GetFragment(tableName string) ([]string, error) {
	numBytes, err := storage.GetValue(Client, "/part_info/"+tableName)
	if err != nil {
		return nil, err
	}
	var s []string
	numStr := byteToString(numBytes)
	num, _ := strconv.Atoi(numStr)
	for i := 1; i <= num; i++ {
		s = append(s, tableName+"."+strconv.Itoa(i))
	}
	return s, nil
}

// Given tableName, return the mode of the table fragment such as V, H
// V 代表垂直分片， H代表水平分片
func GetFragmentMode(tableName string) (string, error) {
	key := "/part_schema/" + tableName
	mode, err := storage.GetValue(Client, key)
	if err != nil {
		return "", err
	}
	return byteToString(mode), err
}

func GetTableColumnExist(tableName string, attr string) (int, error) {
	colBytes, err := storage.GetValue(Client, "/gdd_table/"+tableName)
	if err != nil {
		return -1, err
	}
	s := strings.Split(byteToString(colBytes), ",")
	for _,co:=range s{
		if(co==attr){
			return 1,nil
			break
		}
	}
	return 0,nil
}

func GetFragmentColumnExist(fragmentName string, attr string) (int, error) {//重写
	var Column []string
	mode, err := GetFragmentMode(strings.Split(fragmentName, ".")[0])
	if err != nil {
		return -1, err
	}
	switch mode {
	case "H":
		Column,err=GetTableColumn(strings.Split(fragmentName, ".")[0])
	case "V":
		Column,err=GetVFragmentColumn(strings.Split(fragmentName, ".")[0], fragmentName)
	default:
		return -1, errors.New("找不到分片的属性.")
	}
	for _,co:=range Column{
		if(co==attr){
			return 1,nil
			break
		}
	}
	return 0,errors.New("找不到分片的属性2.")
}

// 返回创建的所有table
func GetTables() ([]string, error) {
	tableStr, err := storage.GetValue(Client, "/gdd_table")
	if err != nil {
		fmt.Println("gdd读取失败.")
		return nil, err
	}
	temp := strings.Split(byteToString(tableStr), ",")
	// FIXME 不超过20个表
	tables := make([]string, 20)
	for index, val := range temp {
		if index >= len(temp)-1 {
			break
		}
		tables[index] = val
	}
	return tables, nil
}

func getFragmentNum(tableName string) (int, error) {
	numbytes, err := storage.GetValue(Client, "/part_info/"+tableName)
	if err != nil {
		fmt.Println("gdd读取失败")
		return -1, err
	}
	return strconv.Atoi(byteToString(numbytes))
}

// 用于判断该分片是否存在
func IsFragExist(tableName string, fragNum int) (bool, error) {
	// 根据分片的表名和号码判断
	tables, err := GetTables()
	if err != nil {
		fmt.Println("gdd读取失败.")
		return false, err
	}
	for _, t := range tables {
		if tableName == t {
			tNum, err := getFragmentNum(t)
			if err != nil {
				return false, err
			}
			if fragNum <= tNum {
				return true, nil
			}
			break
		}
	}
	return false, nil
}

// 如果是垂直分片使用这个函数
// 水平分片返回该分片的条件, 如 id < 1050000
func GetFragmentLimit(tableName string, fragmentName string) (string, error) {
	limit, err := storage.GetValue(Client, "/part_info/"+tableName+"/"+fragmentName)
	if err != nil {
		return "", err
	}
	return byteToString(limit), nil
}

// 返回给定表的主键列名
func GetTableKey(tableName string) (string, error) {
	key := "/gdd_key/" + tableName
	tk, err := storage.GetValue(Client, key)
	if err != nil {
		return "", err
	}
	return byteToString(tk), err
}

// 返回给定表的某一列的数据类型, 如int,char
func GetColumnLimit(tableName string, columnName string) (string, error) {
	limit, err := storage.GetValue(Client, "/gdd_table/"+tableName+"/"+columnName)
	if err != nil {
		return "", err
	}
	return byteToString(limit), nil
}

// 返回给定表名的属性切片
func GetTableColumn(tableName string) ([]string, error) {
	colBytes, err := storage.GetValue(Client, "/gdd_table/"+tableName)
	if err != nil {
		return nil, err
	}
	s := strings.Split(byteToString(colBytes), ",")
	return s, nil
}

// 返回给定表的某一列的数据类型, 如int,char
/*func GetColumnType(svr *server.Server, tableName string, columnName string) (string, error) {
	c := svr.GetClient()
	tp, err := storage.GetValue(c, "/gdd_table/"+tableName+columnName)
	if err != nil {
		return "", nil
	}
	return byteToString(tp), nil
}*/
// 返回该分片所在的站点名
func GetFragmentSite(fragmentName string) (string, error) {
	site, err := storage.GetValue(Client, "/part_site/"+fragmentName)
	if err != nil {
		return "", err
	}
	return byteToString(site), nil
}

func GetFragmentColumn(fragmentName string) ([]string, error) {
	mode, err := GetFragmentMode(strings.Split(fragmentName, ".")[0])
	if err != nil {
		return nil, err
	}
	//fmt.Println(fragmentName)
	//fmt.Println(mode)
	if err != nil {
		return nil, err
	}
	switch mode {
	case "H":
		return GetTableColumn(strings.Split(fragmentName, ".")[0])
	case "V":
		return GetVFragmentColumn(strings.Split(fragmentName, ".")[0], fragmentName)
	default:
		return nil, errors.New("找不到分片的属性.")
	}
}

func GetColumnType(tableName string, colName string) (string, error) {
	val, err := storage.GetValue(Client, "/gdd_table/"+tableName+"/"+colName)
	//fmt.Println(val)
	//fmt.Println(tableName,colName)
	if err != nil {
		//fmt.Println("haha")
		return "", err
	}
	return byteToString(val), nil
}

func GetFragColAndType(fragmentName string) (map[string]string, error) {
	cols, err := GetFragmentColumn(fragmentName)
	if err != nil {
		return nil, err
	}
	tN := strings.Split(fragmentName, ".")[0]
	m := make(map[string]string, len(cols))
	for _, col := range cols {
		m[col], err = GetColumnType(tN, col)
		if err != nil {
			return nil, err
		}
	}
	return m, nil
}

/*
func GetFragmentColumn (svr *server.Server, tableName string) (map[string][]string, error) {
	c := svr.GetClient()
	fragList, err := GetFragment(tableName)
	if err != nil {
		return nil, err
	}
	m := make(map[string][]string)
	mode, err := GetFragmentMode(tableName)
	switch mode {
	case "H":
		for _, frag := range fragList {
			fval, err := addSiteInfo(frag)
			if err != nil {
				return nil, err
			}
			m[fval], err = GetTableColumn(tableName)
			if err != nil {
				return nil, err
			}
		}
	case "V":
		for _, frag := range fragList {
			colsByte, _ := storage.GetValue(c, "/part_info/" + tableName + "/" + frag)
			cols := strings.Split(byteToString(colsByte), ",")
			fval, err := addSiteInfo(frag)
			if err != nil {
				return nil, err
			}
			m[fval] = cols
		}
	}
	return m, nil
}*/

// 给一个水平划分的表名, 返回一个map,key为分片名, value为该分片的条件
// 如Student, 返回[student.1]id<1050000 [student.2]id>=1050000 and id <1100000
// [student.3]id>1100000
func GetHFragAndLimit(tableName string) (map[string]string, error) {
	frags, err := GetFragment(tableName)
	if err != nil {
		return nil, err
	}
	m := make(map[string]string)
	for _, frag := range frags {
		m[frag], err = GetFragmentLimit(tableName, frag)
		if err != nil {
			return nil, err
		}
	}
	return m, nil
}

// 给予一个已知分片模式是垂直分片的分片名和它对应的表名,返回分片属性的字符切片
// 如给Course.1 返回[id,name]的[]string
func GetVFragmentColumn(tableName string, fragment string) ([]string, error) {

	cols, err := GetFragmentLimit(tableName, fragment)
	if err != nil {
		return nil, err
	}
	return strings.Split(cols, ","), nil
}

// 给予一个已知分片模式是垂直分片的分片名和它对应的表名,返回分片属性的字符串
// 如给Course.1 返回id,name的string
func GetVFragColumnString(tableName string, fragment string) (string, error) {
	cols, err := GetFragmentLimit(tableName, fragment)
	if err != nil {
		return "", err
	}
	return cols, nil
}

/*func GetVFragAndCol(svr *server.Server, tableName string) (map[string]string, error) {
	return GetHFragAndLimit(tableName)
}*/

// 将[]byte转换为string
func byteToString(b []byte) string {
	return *(*string)(unsafe.Pointer(&b))
}

func addSiteInfo(frag string) (string, error) {
	site, err := storage.GetValue(Client, "/part_site/"+frag)
	if err != nil {
		return "", err
	}
	var fval string
	lSite := strings.ToLower(byteToString(site))
	switch lSite {
	case "s1":
		fval = frag + ".1"
	case "s2":
		fval = frag + ".2"
	case "s3":
		fval = frag + ".3"
	case "s4":
		fval = frag + ".4"
	default:
		return "", errors.New("找不到分片所在站点.")
	}
	return fval, nil
}


