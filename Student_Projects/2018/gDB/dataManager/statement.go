/*
	statement文件中包含一个statement接口，与几个实现类
	每个类对应一个sql语句类型，每个类用于存储解析出的SQL语句的信息
	其中的ExecStmt()为语句的执行计划(包括ETCD元数据的写入)
*/
package dataManager

import (
	"context"
	"errors"
	"fmt"
	"gDB/exprParser"
	"gDB/gddGet"
	"gDB/pb"
	"gDB/storage"
	"github.com/coreos/etcd/clientv3"
	"github.com/sirupsen/logrus"
	"strconv"
	"strings"
	"sync"
)

type Statement interface {
	PrintStmt()
	ExecStmt() error
}

// DBDDL_Statement

type DBDDL struct {
	Action  string
	DbName  string
	Part_DB []string
}

var localSite string

func Init(name string) {
	localSite = name
}

func (d DBDDL) PrintStmt() {
	fmt.Printf("Action: %v\n", d.Action)
	fmt.Printf("DbName: %v\n", d.DbName)
	fmt.Printf("Part_DB: %v\n", d.Part_DB)
}

func (d DBDDL) ExecStmt() error {
	return nil
}

// DDL_Statement
type DDL struct {
	Action    string
	TableName string
	TableSpec *TableSpec
}

type TableSpec struct {
	Columns []*ColumnDefinition
}

type ColumnDefinition struct {
	Name       string
	DataType   string
	Not_Null   bool
	Primarykey bool
}

func (d DDL) PrintStmt() {
	fmt.Printf("Action: %s\n", d.Action)
	fmt.Printf("TableName: %s\n", d.TableName)
	fmt.Printf("TableSpec:\n")
	for _, col := range d.TableSpec.Columns {
		fmt.Printf("\tName: %s\n", col.Name)
		fmt.Printf("\tDataType: %s\n", col.DataType)
		fmt.Printf("\tNot Null: %v\n", col.Not_Null)
		fmt.Printf("\tPrimaryKey: %v\n", col.Primarykey)
	}
}

// just write gdd
func (d DDL) ExecStmt() error {
	if strings.ToLower(d.Action) == "create" {
		err := writeGddTable(gddGet.Client, &d)
		if err != nil {
			fmt.Println("Write etcd failed!")
			return err
		}
		fmt.Println("Create语句执行成功.")
		return nil
	}
	return nil
}

// Fragment information

type Fragment struct {
	TableName    string
	FragmentMode string // H / V / M
	FragmentNum  int
	FragmentSpec *FragmentSpec
}

type FragmentSpec struct {
	Fragments []*FragmentDefinition
}

type FragmentDefinition struct {
	FragmentName string
	ColumnList   []string
	Limit        string
}

func (f Fragment) PrintStmt() {
	fmt.Printf("TableName: %s\n", f.TableName)
	fmt.Printf("Fragment: %s\n", f.FragmentMode)
	fmt.Printf("Fragment: %d\n", f.FragmentNum)
	fmt.Printf("FragmentSpec:\n")
	for _, frag := range f.FragmentSpec.Fragments {
		fmt.Printf("\tFragmentName: %s\n", frag.FragmentName)
		fmt.Printf("\tColumnList: %v\n", frag.ColumnList)
		fmt.Printf("\tLimit: %s\n", frag.Limit)
	}
}

// write gdd only
func (f Fragment) ExecStmt() error {
	err := writeGddFragment(gddGet.Client, &f)
	if err != nil {
		//fmt.Println("有错")
		//fmt.Println(err)
		return err
	}
	fmt.Println("Fragment语句执行成功。")
	return nil
}

type Allocate struct {
	FragmentName string
	Site         string
}

func (a Allocate) PrintStmt() {
	fmt.Printf("FragmentName: %v\n", a.FragmentName)
	fmt.Printf("Site: %v\n", a.Site)
}

// write gdd and create partition table in target site
func (a Allocate) ExecStmt() error {
	// 首先判断分片是否存在
	//tn := strings.Split(a.FragmentName, ".")[0]
	//num, _ := strconv.Atoi(strings.Split(a.FragmentName, ".")[1])
	/*	b, err := gddGet.IsFragExist(svr, tn, num)
		if err != nil {
			return err
		}
		if !b {
			return errors.New("该分片不存在")
		}*/
	// 1.分片的站点信息写入etcd
	// 2.根据分片信息发送create语句给对应的站点执行
	// step 1
	key1 := "/part_site/" + a.FragmentName
	val1 := a.Site
	err := storage.PutValue(gddGet.Client, key1, val1)
	if err != nil {
		fmt.Println("gdd写入出错.")
		return err
	}
	//step 2
	createSQL, err := FragTableCreate(a.FragmentName)
	if err != nil {
		return err
	}
	// fmt.Println(createSQL)
	// 连接目标站点并执行CREATE语句
	if a.Site == localSite {
		db, err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			fmt.Println("数据库连接失败.")
		}
		_, err = db.Exec(createSQL)
		if err != nil {
			return err
		}
		fmt.Println("allocate语句执行成功")
		return nil
	}
	targetSite, err := storage.Storage.GetRPCClient(a.Site)
	if err != nil {
		fmt.Println(err)
		fmt.Println("目标站点不存在,无法执行allocate语句.")
		return err
	}
	op := fmt.Sprintf("allocate_stmt,%s", createSQL)
	//fmt.Println(op)
	q := peerCommunication.Query{Query: op}
	_, err = targetSite.Client.Execute(context.Background(), &q)
	if err != nil {
		fmt.Println("allocate语句执行失败")
		return err
	}
	fmt.Println("allocate语句执行成功")
	return nil
}

//Load statement
type Load struct {
	tableName string
	filePath  string
}

func (l Load) PrintStmt() {
	fmt.Printf("tableName: %s\n", l.tableName)
	fmt.Printf("filePath: %s\n", l.filePath)
}

// load语句格式: load Student from 'F:\DDBTestFile\student.tsv'
func (l Load) ExecStmt() error {
	//1.现在本地站点建一个临时表temp
	//2.将数据库load进本地数据库
	//3.将数据导出到几个个cst文件,
	//4.再分别发给相应的站点load
	//5.删除临时表和中间tsv文件
	//localSite := "S1"

	//step 1
	tempTableName := l.tableName + "_temp"
	createSQL, err := tempTableCreate(l.tableName, tempTableName) // 创建一个临时表SQL语句
	if err != nil {
		return err
	}
	localLoadSQL := "COPY " + tempTableName + " from" + l.filePath + " delimiter e'\\t';" //将tsv文件导入临时表
	db, err := storage.Storage.DB.Connect("postgres")
	if err != nil {
		fmt.Println("数据库连接失败.")
		return err
	}
	_, err = db.Exec(createSQL) // 本地数据库创建零时表
	if err != nil {
		fmt.Println("load语句的临时表创建失败.")
		return err
	}
	//fmt.Printf("createSQL: %s", createSQL)
	fmt.Println("临时表创建成功.")
	// 退出程序前先删除临时表
	defer func() {
		_, err := db.Exec("drop table " + l.tableName + "_temp")
		if err != nil {
			fmt.Println("临时表删除失败.")
		}
		fmt.Println("临时表删除成功.")
	}()
	result, err := db.Exec(localLoadSQL) // 本地数据库导入数据
	if err != nil {
		return err
	}
	loadRows, _ := result.RowsAffected()
	fmt.Printf("临时表数据导入成功,导入%v行.\n", loadRows)
	mode, err := gddGet.GetFragmentMode(l.tableName) // 导出的tsv文件名为分片名
	if err != nil {
		return err
	}
	m := make(map[string]string)                         // 用来存储分片名与其对于的copy语句
	fragments, _ := gddGet.GetFragment(l.tableName) //copy (select * from table_name where limit) to '' delimiter e'\t'
	switch mode {
	case "H":
		for _, frag := range fragments {
			limit, err := gddGet.GetFragmentLimit(l.tableName, frag)
			if err != nil {
				return err
			}
			/*exportSQL := "copy (select * from " + l.tableName + "_temp where " + limit + ") to '" +
				tempStorageArea + frag + ".tsv' delimiter e'\\t';"
			//fmt.Println(exportSQL)
			m[frag] = exportSQL
			*/
			selectSQL := "select * from " + l.tableName + "_temp where " + limit + ";"
			//fmt.Println(exportSQL)
			m[frag] = selectSQL
		}
	case "V":
		for _, frag := range fragments {
			cols, err := gddGet.GetVFragColumnString(l.tableName, frag)
			if err != nil {
				return err
			}
			/*exportSQL := "copy (select " + cols + " from " + l.tableName + "_temp) to '" +
				tempStorageArea + frag + ".tsv' delimiter e'\\t';"
			//fmt.Println(exportSQL)
			m[frag] = exportSQL*/
			selectSQL := "select " + cols + " from " + l.tableName + "_temp;"
			//fmt.Println(exportSQL)
			m[frag] = selectSQL
		}
	}
	var totalLoadRow int64 = 0
	var waitGroup sync.WaitGroup
	waitGroup.Add(len(m))
	var ch chan int =make(chan int)
	for frag, sql := range m {

		/*result, err := dbLocal.Exec(sql)
		if err != nil {
			return err
		}
		rowExport, err := result.RowsAffected()
		fmt.Printf("%s对应的数据导出成功,导出%v行\n", frag, rowExport)*/
		//fmt.Println(frag)
		//fmt.Println(sql)
		//fmt.Println(m)

		go func(fragmentName string, sql string) {
			defer waitGroup.Done()
			rows, err := db.Query(sql)
			// 结果查询失败
			if err != nil {
				fmt.Println("load执行失败.")
				logrus.Debug(err)
				return
			}
			cols, err := rows.Columns()
			if err != nil {
				logrus.Debug(err)
				return
			}
			//fmt.Println(cols)
			vals := make([][]byte, len(cols))
			scans := make([]interface{}, len(cols))

			for i := range vals {
				scans[i] = &vals[i]
			}
			var results []map[string]string
			for rows.Next() {
				err = rows.Scan(scans...)
				if err != nil {
					logrus.Debug(err)
					return
				}
				row := make(map[string]string)
				for k, v := range vals {
					key := cols[k]
					row[key] = string(v)
				}
				results = append(results, row)
			}
			colTypeMap, err := gddGet.GetFragColAndType(frag)
			//fmt.Println(colTypeMap)
			//return nil
			if err != nil {
				logrus.Debug(err)
				return
			}
			//fmt.Println(frag)
			fNum := strings.Split(frag, ".")[1]
			insertSite, err := gddGet.GetFragmentSite(frag)
			// 使用chan阻塞主线程,防止读到过时的数据
			ch <- 0
			if insertSite == localSite {
				var totalRow int64
				for _, m := range results {
					insertSQL := "insert into " + l.tableName + "_" + fNum + " values("
					for i, col := range cols {
						if colTypeMap[col] == "int" {
							insertSQL += m[col]
							if i < len(cols)-1 {
								insertSQL += ", "
							}
							continue
						}
						insertSQL += "'" + m[col] + "'"
						if i < len(cols)-1 {
							insertSQL += ", "
						}
					}
					insertSQL += ");"
					_, err = db.Exec(insertSQL)
					if err != nil {
						fmt.Println("insert执行失败.")
						logrus.Debug(err)
						return
					}
					totalRow++
				}

				totalLoadRow += totalRow
				fmt.Printf("load%v行记录到本站点%v.\n", totalRow, insertSite)
			} else {
				// 如果load 的数据不在本站点
				var insertSQLStrs string
				for _, m := range results {
					insertSQL := "insert into " + l.tableName + "_" + fNum + " values("
					for i, col := range cols {
						if colTypeMap[col] == "int" {
							insertSQL += m[col]
							if i < len(cols)-1 {
								insertSQL += ", "
							}
							continue
						}
						insertSQL += "'" + m[col] + "'"
						if i < len(cols)-1 {
							insertSQL += ", "
						}
					}
					insertSQL += ");"
					insertSQLStrs += insertSQL + "|"
					//fmt.Printf("%v:%v\n",index, insertSQL)
					//fmt.Printf("成功插入%v行记录到目标站点%v.\n", result.EffectedLine, insertSite)
					//fmt.Printf("共插入%v行记录.\n", result.EffectedLine)
					//fmt.Println("insert语句执行完毕")
					//return nil
				}
				op := fmt.Sprintf("load_stmt|%s", insertSQLStrs)
				q := peerCommunication.Query{Query: op}
				if err != nil {
					logrus.Debug(errors.New("目标站点不存在"))
				}
				targetSite, err := storage.Storage.GetRPCClient(insertSite)
				if err != nil {
					fmt.Printf("目标站点%v连接失败.", insertSite)
					logrus.Debug(err)
					return
				}
				result, err := targetSite.Client.Execute(context.Background(), &q)
				if err != nil {
					fmt.Println("load语句执行失败.")
					logrus.Debug(err)
					return
				}
				totalLoadRow += result.EffectedLine
				fmt.Printf("load%v行记录到站点%v.\n", result.EffectedLine, insertSite)
			}

		}(frag, sql)
		// 阻塞主线程防止读到过时的数据
		<- ch
		}
	waitGroup.Wait()
	fmt.Printf("load执行完毕\n共load%v行记录.\n", totalLoadRow)
	//  删除临时表

	/*for frag := range m {
		tn := strings.Split(frag, ".")[0]
		fNum := strings.Split(frag, ".")[1]
		loadSQL := "copy " + tn + "_" + fNum + " from " + "'" + tempStorageArea + frag + ".tsv' delimiter e'\\t'" //分片对应站点的load语句
		//fmt.Printf("loadSQL: %s", loadSQL)
		loadSite, err := gddGet.GetFragmentSite(svr, frag)
		if err != nil {
			return err
		}

		db, err := database.Connect(loadSite)
		if err != nil {
			return err
		}
		result, err := db.Exec(loadSQL)
		if err != nil {
			return err
		}
		loadRows, _ := result.RowsAffected()
		fmt.Printf("%s load数据成功,成功导入%v行.\n", loadSite, loadRows)
	}
	//TODO 最后还应该删除临时的tsv文件*/
	return nil
}

// Insert_Statement

type Insert struct {
	TableName    string
	InsertMap    map[string]string
	InsertValues []string
}

func (i Insert) PrintStmt() {
	fmt.Printf("TableName: %v\n", i.TableName)
	fmt.Printf("InsertMap: %v\n", i.InsertMap)
	fmt.Println(i.InsertValues)
}

func (i Insert) ExecStmt() error {
	// 访问gdd看有这张表有哪些属性
	// 访问part_info 对db1,db2下分支进行比较
	// 我们假定插入的表都存在
	err := setInsertMap(&i)
	if err != nil {
		return err
	}
	//i.PrintStmt()
	//fmt.Println(i.InsertMap)
	var valStr string
	for index, col := range i.InsertValues {
		valStr += col
		if index < len(i.InsertValues)-1 {
			valStr += ", "
		}
	}
	mode, err := gddGet.GetFragmentMode(i.TableName)
	if err != nil {
		return err
	}
	//dataBase := svr.GetDataBase()
	switch mode {
	// 水平分片的表我们使用exprParser解析每个分片的条件, 如果为真我们就向对应分片的对应站点执行insert语句
	case "H":
		m, err := gddGet.GetHFragAndLimit(i.TableName)
		if err != nil {
			return err
		}
		for frag, limit := range m {
			//fmt.Println(frag)
			//fmt.Println(limit)
			r, _ := exprParser.Parse(strings.NewReader(limit), i.TableName, i.InsertMap)
			if r {
				//fmt.Println(r)
				//fmt.Println(frag)
				insertSite, err := gddGet.GetFragmentSite(frag)
				if err != nil {
					return err
				}
				//insertDB, err := dataBase.Connect(insertSite)
				//if err != nil {
				//	return err
				//}
				// exec SQL
				insertSQL := "insert into " + i.TableName + "_" + strings.Split(frag, ".")[1] +
					" values(" + valStr + ");"
				//fmt.Println(insertSQL)
				fmt.Println(frag)
				fmt.Println(insertSQL)
				fmt.Println(insertSite)
				if insertSite == localSite {
					db, err := storage.Storage.DB.Connect("postgres")
					if err != nil {
						fmt.Println("数据库连接失败.")
					}
					result, err := db.Exec(insertSQL)
					if err != nil {
						return err
					}
					fmt.Println("insert语句执行成功.")
					rowNum, _ := result.RowsAffected()
					fmt.Printf("插入%v行记录到本站点%s.\n", rowNum, insertSite)
					//fmt.Printf("共插入%v行记录.\n", rowNum)
					fmt.Println("insert语句执行完毕.")
					return nil
				}
				op := fmt.Sprintf("insert_stmt,%s", insertSQL)
				q := peerCommunication.Query{Query: op}
				targetSite, err := storage.Storage.GetRPCClient(insertSite)
				if err != nil {
					return errors.New("目标站点不存在")
				}
				result, err := targetSite.Client.Execute(context.Background(), &q)
				if err != nil {
					fmt.Println("insert语句执行失败.")
					return err
				}
				fmt.Printf("插入%v行记录到目标站点%v.\n", result.EffectedLine, insertSite)
				//fmt.Printf("共插入%v行记录.\n", result.EffectedLine)
				fmt.Println("insert语句执行完毕.")
				return nil
			}
		}

	case "V":
		frags, err := gddGet.GetFragment(i.TableName)
		if err != nil {
			return err
		}
		var totalRow int64
		for _, frag := range frags {
			insertSQL := "insert into " + i.TableName + "_" + strings.Split(frag, ".")[1] + " values("
			cols, err := gddGet.GetVFragmentColumn(i.TableName, frag)
			if err != nil {
				return err
			}
			for index, col := range cols {
				insertSQL += i.InsertMap[col]
				if index < len(cols)-1 {
					insertSQL += ", "
				}
			}
			insertSQL += ");"
			//fmt.Println(insertSQL)
			insertSite, err := gddGet.GetFragmentSite(frag)
			//fmt.Println(insertSite)
			if err != nil {
				return err
			}
			if insertSite == localSite {
				db, err := storage.Storage.DB.Connect("postgres")
				if err != nil {
					fmt.Println("数据库连接失败.")
					return err
				}
				result, err := db.Exec(insertSQL)
				if err != nil {
					return err
				}
				rowsInsert, _ := result.RowsAffected()
				totalRow += rowsInsert
				fmt.Printf("插入%v行记录到本站点%v.\n", rowsInsert, insertSite)
				continue
			}
			// 非本站点语句执行
			op := fmt.Sprintf("insert_stmt,%s", insertSQL)
			q := peerCommunication.Query{Query: op}
			targetSite, err := storage.Storage.GetRPCClient(insertSite)
			if err != nil {
				return errors.New("目标站点连接失败!")
			}
			result, err := targetSite.Client.Execute(context.Background(), &q)
			if err != nil {
				fmt.Println("insert语句执行失败!")
				return err
			}
			//fmt.Println("insert语句执行成功.")
			fmt.Printf("插入%v行记录到目标站点%v\n.", result.EffectedLine, insertSite)
			totalRow += result.EffectedLine
			//return nil
		}
		fmt.Printf("共插入%v行数据\n", totalRow)
		fmt.Println("insert语句执行完毕.")
		return nil
	default:
		err := errors.New("插入失败:)")
		return err
	}
	return nil
}

// Delete_Statement

type Delete struct {
	TargetName string // 要删除数据的表名
	Condition  string // delete 时我们将语句发送给所有站点
	DeleteCols string // 这个属性用于delete垂直划分的表且含where条件, where 后面只接一个属性的条件
}

func (d Delete) PrintStmt() {
	fmt.Printf("TargetName: %v\n", d.TargetName)
	fmt.Printf("Condition: %v\n", d.Condition)
	fmt.Printf("DeleteCols: %v\n", d.DeleteCols)
}

func (d Delete) ExecStmt() error {
	// 这个做法比较偷懒了 :)
	frags, err := gddGet.GetFragment(d.TargetName)
	if err != nil {
		return err
	}
	//database := svr.GetDataBase()
	var totalRow int64
	totalRow = 0
	// 在yacc中我们设置如何delete where为空（即没有where）我们将condition置为空
	if d.Condition == "*" {
		for _, frag := range frags {
			deleteSQL := "delete from " + d.TargetName + "_" + strings.Split(frag, ".")[1] + ";"
			//fmt.Println(deleteSQL)
			deleteSite, err := gddGet.GetFragmentSite(frag)
			//fmt.Println(dbSite)
			if err != nil {
				return err
			}
			// 如果目标站点在本地
			if deleteSite == localSite {
				db, err := storage.Storage.DB.Connect("postgres")
				if err != nil {
					return err
				}
				result, err := db.Exec(deleteSQL)
				if err != nil {
					fmt.Println("delete语句执行失败")
					return err
				}
				rows, _ := result.RowsAffected()
				if rows > 0 {
					fmt.Printf("删除本站点%v中%v行记录.\n", deleteSite, rows)
					totalRow += rows
				}
				continue
			}
			targetSite, err := storage.Storage.GetRPCClient(deleteSite)
			if err != nil {
				fmt.Printf("目标站点%v连接失败!", deleteSite)
				return err
			}
			op := fmt.Sprintf("delete_stmt,%s", deleteSQL)
			q := peerCommunication.Query{Query: op}
			result, err := targetSite.Client.Execute(context.Background(), &q)
			if err != nil {
				fmt.Println("delete语句执行失败!")
				return err
			}
			if result.EffectedLine > 0 {
				fmt.Printf("删除站点%v中%v行记录.\n", deleteSite, result.EffectedLine)
				totalRow += result.EffectedLine
			}
		}
		fmt.Printf("总删除%v行记录.\n", totalRow)
		fmt.Println("delete语句执行完毕.")
		return nil
	}
	mode, err := gddGet.GetFragmentMode(d.TargetName)
	if err != nil {
		return err
	}
	switch mode {
	case "H":
		for _, frag := range frags {
			deleteSQL := "delete from " + d.TargetName + "_" + strings.Split(frag, ".")[1] +
				" where " + d.Condition
			//fmt.Println(deleteSQL)
			deleteSite, err := gddGet.GetFragmentSite(frag)
			if err != nil {
				return err
			}
			if deleteSite == localSite {
				db, err := storage.Storage.DB.Connect("postgres")
				if err != nil {
					fmt.Println("数据库连接失败!")
					return err
				}
				result, err := db.Exec(deleteSQL)
				if err != nil {
					fmt.Println(err)
					return err
				}
				rows, _ := result.RowsAffected()
				totalRow += rows
				if rows > 0 {
					fmt.Printf("删除本站点%v中%v行数据.\n", deleteSite, rows)
					return nil
				}
				continue
			}
			op := fmt.Sprintf("delete_stmt,%s", deleteSQL)
			q := peerCommunication.Query{Query: op}
			targetSite, err := storage.Storage.GetRPCClient(deleteSite)
			if err != nil {
				fmt.Println("目标站点连接失败.")
				return err
			}
			result, err := targetSite.Client.Execute(context.Background(), &q)
			if err != nil {
				return err
			}
			rows := result.EffectedLine
			totalRow += rows
			if rows > 0 {
				fmt.Printf("删除站点%v中%v行记录.\n", deleteSite, rows)
				fmt.Println("delete语句执行完毕.")
				return nil
			}
			//if rows == 0 {
			//	fmt.Printf("%v: haha :)", deleteSite)
			//}
		}
		fmt.Printf("删除失败.")
		return errors.New("记录删除失败")
		// FIXME:对于这种情况, 目前我们只考虑where后面接单个属性的条件, 并且只能包含一个and 如 id < 6 and id > 3
	case "V":
		key, err := gddGet.GetTableKey(d.TargetName) //FIXME: 我们只考虑单个key的情况
		if err != nil {
			return err
		}
		condCols := strings.Split(d.DeleteCols, ",")
		if len(condCols) == 2 {
			if condCols[1] != condCols[0] {
				return errors.New("垂直划分的表的delete语句的where只能包含一个属性:)")
			}
		} else if len(condCols) > 2 {
			return errors.New("垂直划分的表的delete语句的where只能包含一个and:)")
		}
		// 如果删除条件的属性等于主键, 我们发送这个语句到所有站点
		// 等于主键, 注意主键也只是单属性
		if condCols[0] == key {
			for _, frag := range frags {
				deleteSQL := "delete from " + d.TargetName + "_" + strings.Split(frag, ".")[1] +
					" where " + d.Condition + ";"
				//fmt.Println(deleteSQL)
				deleteSite, err := gddGet.GetFragmentSite(frag)
				if err != nil {
					return err
				}
				if deleteSite == localSite {
					db, err := storage.Storage.DB.Connect("postgres")
					if err != nil {
						fmt.Println("数据库连接出错.")
						return err
					}
					result, err := db.Exec(deleteSQL)
					if err != nil {
						fmt.Println("delete执行失败")
						return err
					}
					rows, _ := result.RowsAffected()
					totalRow += rows
					fmt.Printf("删除本站点%v中%v行记录\n.", deleteSite, rows)
					continue
				}
				targetSite, err := storage.Storage.GetRPCClient(deleteSite)
				if err != nil {
					fmt.Println("目标站点%v连接失败", deleteSite)
					return err
				}
				op := fmt.Sprintf("delete_stmt,%s", deleteSQL)
				q := peerCommunication.Query{Query: op}
				result, err := targetSite.Client.Execute(context.Background(), &q)
				rows := result.EffectedLine
				totalRow += rows
				fmt.Printf("删除站点%v中%v行记录\n", deleteSite, rows)
			}
			fmt.Println("delete执行完毕.")
			fmt.Printf("共删除%v行记录.\n", totalRow)
			return nil
		}
		// 如果删除条件属性不是主键,那么我们selecet出属性的值,再根据主键删除对应各个站点上的数据
		if condCols[0] != key {
			var targetFrag string // 非主属性所在的分片
			frags, err := gddGet.GetFragment(d.TargetName)
			if err != nil {
				return err
			}
		loop:
			for _, frag := range frags {
				cols, err := gddGet.GetVFragmentColumn( d.TargetName, frag)
				if err != nil {
					return err
				}
				for _, col := range cols {
					if col == condCols[0] {
						targetFrag = frag
						break loop
					}
				}
			}
			selectSQL := "select " + key + " from " + d.TargetName + "_" + strings.Split(targetFrag, ".")[1] +
				" where " + d.Condition + ";"
			//fmt.Println(selectSQL)
			op := fmt.Sprintf("dselect_stmt,%s", selectSQL)
			q := peerCommunication.Query{Query: op}
			selectSite, err := gddGet.GetFragmentSite( targetFrag)
			if err != nil {
				return err
			}
			targetSite, err := storage.Storage.GetRPCClient(selectSite)
			if err != nil {
				fmt.Println("站点%v连接失败")
				return err
			}
			result, err := targetSite.Client.Execute(context.Background(), &q)
			// 这里我们将effected line 当作返回的主键值
			id := result.EffectedLine
			for _, frag := range frags {
				deleteSQL := "delete from " + d.TargetName + "_" + strings.Split(frag, ".")[1] +
					" where " + key + " = " + strconv.Itoa(int(id)) + ";"
				deleteSite, err := gddGet.GetFragmentSite( frag)
				if err != nil {
					return err
				}
				if deleteSite == localSite {
					db, err := storage.Storage.DB.Connect("postgres")
					if err != nil {
						fmt.Println("数据库连接失败.")
						return err
					}
					result, err := db.Exec(deleteSQL)
					if err != nil {
						fmt.Println("delete执行失败.")
						return err
					}
					rows, _ := result.RowsAffected()
					fmt.Printf("删除本站点%v%v行数据.\n", deleteSite, rows)
					totalRow += rows
					continue
				}
				targetSite, err := storage.Storage.GetRPCClient(deleteSite)
				if err != nil {
					fmt.Println("目标站点连接失败.")
					return err
				}
				op := fmt.Sprintf("delete_stmt,%s", deleteSQL)
				q := peerCommunication.Query{Query: op}
				result, err := targetSite.Client.Execute(context.Background(), &q)
				rows := result.EffectedLine
				totalRow += rows
				fmt.Printf("删除目标站点%v上%v行记录", deleteSite, rows)
			}
			fmt.Println("delete语句执行完毕")
			fmt.Printf("共删除%v行记录", totalRow)
			return nil

			/*targetDB, err := database.Connect(targetSite)
			if err != nil {
				return err
			}
			rows, err := targetDB.Query(selectSQL)
			//defer rows.Close()	// remember close the door

			if err != nil {
				return err
			}
			// ids 我们用来存储选择的主键结果
			var ids []string
			for rows.Next() {
				var id string
				rows.Scan(&id)
				fmt.Println(id)
				ids = append(ids, id)
			}
			err = rows.Err()
			if err != nil {
				return err
			}
			//fmt.Println(ids)
			fmt.Printf("每个站点需要删除的记录数为%v.\n", len(ids))
			// 我们构造 delete from frag where key in ()
			idString := "(" // (1, 2, 3, 4)
			for index, id := range ids {
				idString += id
				if index < len(ids)-1 {
					idString += ", "
				}
			}
			idString += ");"
			// 创建一个waitGroup, 体验一下go语言的goroutine :)
			var waitGroup sync.WaitGroup
			waitGroup.Add(len(frags))
			for _, frag := range frags {
				go func(frag string, database *storage.Database) {
					deleteSQL := "delete from " + d.TargetName + "_" + strings.Split(frag, ".")[1] +
						" where " + key + " in " + idString
					//fmt.Println(deleteSQL)
					deleteSite, err := gddGet.GetFragmentSite(svr, frag)
					if err != nil {
						log.Fatal(err)
						return
					}
					dbDelete, err := database.Connect(deleteSite)
					if err != nil {
						log.Fatal(err)
						return
					}
					result, err := dbDelete.Exec(deleteSQL)
					if err != nil {
						log.Fatal(err)
						return
					}
					r, _ := result.RowsAffected()
					totalRow += r
					fmt.Printf("成功删除站点%v上%v行记录.\n", deleteSite, r)
					waitGroup.Done()
				}(frag, database)
			}
			// 等待所有站点的delete执行完毕
			go func() {
				waitGroup.Wait()
				fmt.Printf("总共删除%v行记录.\n", totalRow)
			}()
			return nil
			*/
		}
	}

	//mode, err := gddGet.GetFragmentMode(svr, d.TargetName)
	//if err != nil {
	//	return err
	//}
	return nil
}

// Update_Statement

type Update struct {
}

// create table 语句时写入etcd
func writeGddTable(c *clientv3.Client, d *DDL) error {
	// 1 /gdd_table student,
	key0 := "/gdd_table"
	val0 := d.TableName + ","
	err := storage.PutValue(c, key0, val0)
	if err != nil {
		fmt.Println("gdd写入失败.")
		return err
	}
	// 2 /gdd_table/student: id,name,email
	key1 := fmt.Sprintf("/gdd_table/%s", d.TableName) //
	var val1 string
	for index, col := range d.TableSpec.Columns {
		val1 += col.Name
		if index < len(d.TableSpec.Columns)-1 {
			val1 += ","
		}
	}
	err = storage.PutValue(c, key1, val1)
	if err != nil {
		fmt.Println("gdd写入失败.")
		return err
	}
	// 3 写入 /gdd_table/student/id: int ....

	for _, col := range d.TableSpec.Columns {
		key2 := fmt.Sprintf("%v/%v", key1, col.Name)
		val2 := col.DataType
		err := storage.PutValue(c, key2, val2)
		if err != nil {
			fmt.Println("gdd写入失败.")
			return err
		}
	}

	// 4. 写入 /gdd_key/student:
	// FIXME 目前我们默认key只有单个属性
	key3 := fmt.Sprintf("/gdd_key/%v", d.TableName)
	var val3 string
	var keys []string
	for _, col := range d.TableSpec.Columns {
		if col.Primarykey {
			keys = append(keys, col.Name)
		}
	}
	for index, val := range keys {
		val3 += val
		if index < (len(keys) - 1) {
			val3 += ","
		}
	}
	err = storage.PutValue(c, key3, val3)
	if err != nil {
		fmt.Println("gdd写入失败.")
		return err
	}
	return nil
}

// fragment语句 写入etcd
func writeGddFragment(c *clientv3.Client, f *Fragment) error {
	mode := f.FragmentMode
	switch mode {
	case "H":
		//1. 写入 /part_schema/student: H
		//2. 写入 /part_schema/student/H: id
		//3. 写入 /part_info/student: 2
		//4. 写入 /part_info/student/student.1: id<20
		//						    /student.2: id>=20
		key1 := "/part_schema/" + f.TableName
		val1 := "H"
		key2 := key1 + "/H"
		var val2 string
		cols := f.FragmentSpec.Fragments[0].ColumnList
		if len(cols) == 1 {
			val2 = cols[0]
		} else if len(cols) == 2 {
			if cols[0] == cols[1] {
				val2 = cols[1]
			} else {
				val2 = cols[0] + "," + cols[1]
			}
		} else {
			err := errors.New("水平分片的属性不能超过2个:).")
			return err
		}
		key3 := "/part_info/" + f.TableName
		val3 := strconv.Itoa(f.FragmentNum)

		err := storage.PutValue(c, key1, val1)
		if err != nil {
			fmt.Println("gdd 写入失败.")
			return err
		}
		err = storage.PutValue(c, key2, val2)
		if err != nil {
			fmt.Println("gdd 写入失败.")
			return err
		}
		err = storage.PutValue(c, key3, val3)
		if err != nil {
			fmt.Println("gdd 写入失败.")
			return err
		}
		for _, frag := range f.FragmentSpec.Fragments {
			key4 := key3 + "/" + frag.FragmentName
			val4 := frag.Limit
			err := storage.PutValue(c, key4, val4)
			if err != nil {
				fmt.Println("gdd 写入失败.")
				return err
			}
		}
		return nil
	case "V":
		key1 := "/part_schema/" + f.TableName
		val1 := "V"

		key2 := "/part_info/" + f.TableName
		val2 := strconv.Itoa(f.FragmentNum)

		err := storage.PutValue(c, key1, val1)
		if err != nil {
			fmt.Println("gdd写入失败.")
			return err
		}
		err = storage.PutValue(c, key2, val2)
		if err != nil {
			fmt.Println("gdd写入失败.")
			return err
		}

		for _, frag := range f.FragmentSpec.Fragments {
			key3 := key2 + "/" + frag.FragmentName
			var val3 string
			for index, val := range frag.ColumnList {
				val3 += val
				if index < (len(frag.ColumnList) - 1) {
					val3 += ","
				}
			}
			err := storage.PutValue(c, key3, val3)
			if err != nil {
				fmt.Println("gdd写入失败.")
				return err
			}
		}
		return nil
	default:
		err := errors.New("很抱歉我们支持水平分片或者垂直分片!")
		return err
	}
	return nil
}

// load 语句创建临时表的sql语句构建
func tempTableCreate(tableName string, newName string) (string, error) {
	tk, err := gddGet.GetTableKey(tableName)
	if err != nil {
		return "", err
	}
	cols, err := gddGet.GetTableColumn(tableName)
	if err != nil {
		return "", err
	}
	createSQL := "create table " + newName + "("
	for index, col := range cols {
		limit, err := gddGet.GetColumnLimit(tableName, col)
		if err != nil {
			return "", err
		}
		createSQL += col + " " + limit
		if col == tk {
			createSQL += " primary key "
		}
		if index < len(cols)-1 {
			createSQL += ","
		}
	}
	createSQL += ");"
	return createSQL, nil
}

// allocate 语句创建小表的SQL语句
func FragTableCreate(fragmentName string) (string, error) {
	tN := strings.Split(fragmentName, ".")[0]   // student.1 -> student
	fNum := strings.Split(fragmentName, ".")[1] // student.1 -> 1
	mode, err := gddGet.GetFragmentMode(tN)
	if err != nil {
		return "", nil
	}
	var s string
	switch mode {
	case "H":
		if err != nil {
			return "", err
		}
		s, err = tempTableCreate(tN, tN+"_"+fNum)
		if err != nil {
			return "", nil
		}
	case "V":
		s = "create table " + tN + "_" + fNum + "( "
		cols, err := gddGet.GetVFragmentColumn(tN, fragmentName)
		if err != nil {
			return "", err
		}
		tk, err := gddGet.GetTableKey(tN)
		if err != nil {
			return "", err
		}
		for index, col := range cols {
			limit, err := gddGet.GetColumnLimit(tN, col)
			if err != nil {
				return "", err
			}
			s += col + " " + limit
			if col == tk {
				s += " primary key "
			}
			if index < (len(cols) - 1) {
				s += ","
			}
		}
		s += ");"
	}
	return s, nil
}

// 将插入数据设置成 [id]: 19 [name]: shen , ...这种格式
func setInsertMap(i *Insert) error {
	cols, err := gddGet.GetTableColumn(i.TableName)
	if err != nil {
		return err
	}
	i.InsertMap = make(map[string]string)
	for index, col := range cols {
		i.InsertMap[col] = i.InsertValues[index]
	}
	return nil
}
