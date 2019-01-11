/*
	**  没有和代码之前用来测试的
 */
package main

func main() {

	//sql := "create table test(id int key, name char(20) not null, email char(50) primary key); "
	//sql := "fragment Student horizontally into id<105000, id>=105000 and id<110000, id>=110000;"
	//sql := "fragment Teacher horizontally into id<201000 and title<>3, id<201000 and title=3,"+
	//	   "id>=201000 and title<>3, id>=201000 and title=3;"

	//sql := "fragment Course vertically into (id, name), (id, location, credit_hour, teacher_id);"
	//sql := "fragment Course vertically into (id, name), (id, location, credit_hour, teacher_id)"
	//sql := "insert into test values(12,  'shen yunlong',  'shen@ruc.edu.cn' );"
	//sql := "delete from test where id > 3 and name = 'shen'; "
	//sql := "create database test;"
	//sql := "allocate Student.1 to S1;"
	//sql := "load student from 'C:/Users/testUser/Desktop/TSV/ratings.list.tsv';"

	//	sql := "create table Student (id int key, name char(25), sex char(1), age int, degree int);"

	//stmt, err := dataManager.Parse(strings.NewReader(sql))
	//if err != nil {
	//	fmt.Println(err)
	//	return
	//}
	//stmt.PrintStmt()
	//err = stmt.ExecStmt()
	//if err != nil {
	//	fmt.Println(err)
	//}
	//fmt.Println("Executed.")
	// 使用append 避免空指针的情况

	/*s := *[]string
	s[0] = "ab"
	s[1] =  "cd"
	fmt.Println(s)*/

	/*s := bufio.NewReader(strings.NewReader("123"))
		var buf bytes.Buffer
	    r, _, _ := s.ReadRune()
		buf.WriteRune(r)
		t, _, _ := s.ReadRune()
		buf.WriteRune(t)
		q, _, _ := s.ReadRune()
		buf.WriteRune(q)
	    num, _ := strconv.Atoi(buf.String())
	    fmt.Println(num)
		z, _ := strconv.Atoi("(")
	    fmt.Printf("%d", z)*/
	/*
	   s := "2"
	   strings.ToLower(s)
	   fmt.Println(s)*/
	//fn := true && false
	//fmt.Println(fn)
	//m := make(map[string]string)
	//m = nil
	/*var a interface{}
	  a = 6
	  fmt.Println(a)*/
}
