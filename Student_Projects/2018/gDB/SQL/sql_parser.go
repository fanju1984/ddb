/*
SQL语句的解析、优化
 */
package SQL

import (
	"fmt"
	"gDB/gddGet"
	"regexp"
	//"regexp/syntax"

	//"strconv"
	"strings"
	log "github.com/sirupsen/logrus"
)

func Reg_T(Sql_query string, Start string, End string) []string {
	exp_p := regexp.MustCompile(Start + "(.)*?" + End)
	all := exp_p.FindAllString(Sql_query, -1)
	if all == nil {
		return nil
	}
	all[0] = strings.Replace(all[0], Start+" ", "", 1)
	all[0] = strings.Replace(all[0], End, "", 1)
	all[0] = strings.Replace(all[0], " ", "", 1)
	sec := strings.Split(all[0], ",")
	for _, value := range sec { //假设
		value = strings.Replace(value, " ", "", -1)
	}
	return sec
}
func Reg_T2(Sql_query string, Start string) []string {
	exp_p := regexp.MustCompile(Start + "(.*)?")
	all := exp_p.FindAllString(Sql_query, -1)
	if all == nil {
		return nil
	}
	all[0] = strings.Replace(all[0], Start+" ", "", 1)
	all[0] = strings.Replace(all[0], " ", "", 1)
	sec := strings.Split(all[0], ",")
	for _, value := range sec { //假设
		value = strings.Replace(value, " ", "", -1)
	}
	return sec
}
func Reg_P(Sql_query string, Start string, End string) []string {
	exp_p := regexp.MustCompile(Start + "(.)*?" + End)
	all := exp_p.FindAllString(Sql_query, -1)
	if all == nil {
		return nil
	}
	all[0] = strings.Replace(all[0], Start+" ", "", 1)
	all[0] = strings.Replace(all[0], End, "", 1)
	all[0] = strings.Replace(all[0], " ", "", 1)
	var res []string = make([]string, 1)
	res[0] = all[0]
	return res
}

func Reg_JS(Sql_query string) ([]string, []string) {
	index := strings.Index(Sql_query, "where")
	if index == -1 {
		return nil, nil
	}
	all := Sql_query[index:]
	//	fmt.Print(all)
	all = strings.Replace(all, "where ", "", 1)
	all = strings.Replace(all, " ", "", 1)
	sec := strings.Split(all, "and")
	var j []string = make([]string, 0)
	var s []string = make([]string, 0)
	for _, vaule := range sec {
		exp := regexp.MustCompile("\\.(.)*?\\.") //换为..
		a := exp.FindAllString(vaule, -1)
		if a == nil {
			s = append(s, vaule)
		} else {
			j = append(j, vaule)
		}
	}

	return j, s

}

func Parser(Sql_query string) *SQL_query {
	var Sql_res *SQL_query
	var pro *Projection
	var tab *Table
	var jo *Join
	var sel *Select
	//pro = NewPro(Reg_T(Sql_query, "select", "from"))
	//tab = NewTab(Reg_T(Sql_query, "from", "where"))

	//Projection!!!
	proStr := Reg_P(Sql_query, "select", "from")
	pro = NewPro(proStr)
	//TABLE!!!
	var tabs []string
	if strings.Contains(Sql_query, "where") {
		tabs = Reg_T(Sql_query, "from", "where")
	} else {
		tabs = Reg_T2(Sql_query, "from")
	}
	var DTable []Distributed_Table = make([]Distributed_Table, 0)
	for _, value := range tabs { //假设
		value:= strings.Replace(value, " ", "", -1)
		var segs []string
		var err error
		segs,err=gddGet.GetFragment(value)
		if(err!=nil){
			log.Errorf("error_SendResult error is %v", err)
		}
		var segement []string = make([]string, 0)
		var DTab Distributed_Table
		for _,segss:=range segs{
			fmt.Println("seg_name:"+segss)
			site_name,_:=gddGet.GetFragmentSite(segss)
			fmt.Println("site_name:"+site_name)
			site_name=strings.Replace(site_name,"st","",-1)
			fmt.Println("cmk: "+segss+"."+site_name)
			segement = append(segement, segss+"."+site_name)
		}
		HV_mode,_:=gddGet.GetFragmentMode(value)
		fmt.Println("HV_mode: "+HV_mode)
		if(HV_mode=="H"){
			DTab = NewDTab(value, segement, 0)
		}else{
			DTab = NewDTab(value, segement, 1)
		}
		DTable = append(DTable, DTab)
	}
	tab = NewTab(DTable)
	//Join!!!
	a, b := Reg_JS(Sql_query)
	var joins []Join_each = make([]Join_each, 0)
	for _, value := range a { //假设
		value = strings.Replace(value, " ", "", -1)
		sec := strings.Split(value, ".")
		A := sec[0]
		A = strings.Replace(A, " ", "", -1)
		C := sec[2]
		C = strings.Replace(C, " ", "", -1)
		B := sec[1]
		B = strings.Replace(B, " ", "", -1)
		sec2 := strings.Split(B, "=")
		B2 := sec2[1]
		joine := NewEJoin(A, B2, C)
		joins = append(joins, joine)
	}
	jo = NewJoin(joins)
	//SELECT!!!
	var sels []Select_eachT = make([]Select_eachT, tab.size) //一个table一个Select_eachT,一个select_Each[]数组
	//初始化sels
	for i := 0; i < tab.size; i++ {
		sels[i].size = 0
		sels[i].sel = make([]Select_Each, 0)
	}
	//选择运算符
	var pos []int = make([]int, 0)
	/*
		for i, _ := range b {
			//fmt.Println(value)
			pos = append(pos, i)
		}*/
	var tabS []string=make([]string,0)
	var mm int
	mm=0
	for _, bi := range b {
		t:=strings.Split(bi,".")[0]
		t=strings.Replace(t," ","",-1)
		var x int
		var tabss string=""
		var flag int=0
		for x,tabss=range tabS{
			if(tabss==t){
				flag=1
				break
			}
		}
		if(flag==0){
			tabS= append(tabS, t)
			pos = append(pos, mm)
			mm++
		}else{
			pos=append(pos,x)
		}
	}
	/*
	for i:=0;i<len(b);i++{
		t:=strings.Split(b[0],".")[0]
		pos = append(pos, m)
		m++
	}*/
	for m, value := range b { //假设
		value = strings.Replace(value, " ", "", -1)
		var operas = [6]string{"<>",">=", ">", "<=", "<","="}

		for _, opera := range operas {
			if strings.Index(value, opera) != -1 {
				sec := strings.Split(value, opera)
				sec[0] = strings.Replace(sec[0], " ", "", -1)
				sec[1] = strings.Replace(sec[1], " ", "", -1)
				sele := NewSelectEach(sec[0], opera, sec[1])
				sels[pos[m]].size++
				sels[pos[m]].pos = pos[m]
				sels[pos[m]].sel = append(sels[pos[m]].sel, sele)
				break
			}
		}
	}
	sel = NewSel(sels)
	Sql_res = NewSQL(pro, tab, jo, sel)
	return Sql_res
}
func getPos(tables []Distributed_Table, target string) int {
	for i := 0; i < len(tables); i++ { //初始化二维数组
		//fmt.Print("TABLES:" + tables[i])
		//fmt.Println("TARGET:" + target)
		if tables[i].tab == target {
			return i
			break
		}
	}
	return -1
}

/*
func update(par *SQL_query) *SQL_query {
	var pros []string = par.pro.pro
	var sels []string = par.sel.sel
	var joins []string = par.jo.jo

	for i, vaule := range pros {
		pros[i] = strings.Replace(vaule, " ", "", -1)
	}
	for i, vaule := range sels {
		sels[i] = strings.Replace(vaule, " ", "", -1)
	}
	for i, vaule := range joins {
		joins[i] = strings.Replace(vaule, " ", "", -1)
	}
	return par
}
*/
func IN(i int, tabTemp []int) int {
	if len(tabTemp) == 0 {
		return -1
	}
	for j, _ := range tabTemp {
		if tabTemp[j] == i {
			return 0
		}
	}
	return 1
}

func contain(i int, joinElem []int) int {
	for j := 0; j < len(joinElem); j++ {
		if i == joinElem[j] {
			return 1
		}
	}
	return 0
}
func INJOIN(joinTemp []int, a string, b string, joins []Join_each) int { //join第几条
	for i := 0; i < len(joins); i++ {
		if contain(i, joinTemp) == 0 { //没用过这句Join:  joins[i]
			/*
				if strings.Contains(joins[i], a) && strings.Contains(joins[i], b) {
					return i
				}*/
			if (joins[i].tab1 == a && joins[i].tab2 == b) || (joins[i].tab2 == a && joins[i].tab1 == b) {
				return i
			}
		}
	}
	return -1

}
func updateOrderofTJ(par *SQL_query) *SQL_query {
	var tableJ [][]int
	var tables []Distributed_Table = par.tab.tab
	var joins []Join_each = par.jo.jo
	for i := 0; i < par.tab.size; i++ { //初始化二维数组
		tmp := make([]int, par.tab.size)
		tableJ = append(tableJ, tmp)
	}
	for _, value := range joins {
		pos1 := getPos(tables, value.tab1)
		pos2 := getPos(tables, value.tab2)
		tableJ[pos1][pos2] = 1
		tableJ[pos2][pos1] = 1
	}
	//size := 0
	start := 0
	var tabTemp []int = make([]int, 0)
	tabTemp = append(tabTemp, start)
	for {
		flag := 0
		for i := 0; i < par.tab.size; i++ {
			if tableJ[start][i] == 1 {
				if IN(i, tabTemp) == 1 {
					//size++
					tabTemp = append(tabTemp, i)
					start = i
					flag = 1
					break
				}
			}
		}

		if flag == 0 { //需要回溯
			fmt.Println("回溯")
			for j := len(tabTemp) - 2; j >= 0; j-- {
				for i := 0; i < par.tab.size; i++ {
					if tableJ[tabTemp[j]][i] == 1 {
						if IN(i, tabTemp) == 1 {
							//size++
							tabTemp = append(tabTemp, i)
							start = i
							flag = 1
							break
						}
					}
				}
				if flag == 1 {
					break
				}
			}
		}
		//fmt.Println(size)
		//size++
		if len(tabTemp) >= par.tab.size {
			break
		}
	}
	var table2 []Distributed_Table = make([]Distributed_Table, 0)
	for _, vaule := range tabTemp {
		table2 = append(table2, tables[vaule])
	}
	for i, vaule := range table2 { //更新table完成
		par.tab.tab[i] = vaule
	}
	/*
		for _, vaule := range par.tab.tab {
			fmt.Println(vaule)
		}*/
	//更新JOIN
	start = 0
	//var joinElem []string = make([]string, 0)
	var joinTemp []int = make([]int, 0)
	for j := 0; j < par.tab.size-1; j++ {
		a := par.tab.tab[j].tab
		b := par.tab.tab[j+1].tab
		tem := INJOIN(joinTemp, a, b, par.jo.jo)
		if tem == -1 { //回溯
			for k := j - 1; k >= 0; k-- {
				a = par.tab.tab[k].tab
				tem = INJOIN(joinTemp, a, b, par.jo.jo)
				if tem != -1 {
					break
				}
			}
		}
		joinTemp = append(joinTemp, tem)
		if len(joinTemp) >= par.jo.size {
			break
		}
		//fmt.Println("a: " + a + ";b: " + b)

	}
	/*
		for _, vaule := range joinTemp {
			fmt.Println(vaule)
		}
		for _, vaule := range joins {
			fmt.Println(vaule)
		}*/

	var join2 []Join_each = make([]Join_each, 0)
	for _, vaule := range joinTemp {
		join2 = append(join2, joins[vaule])
	}
	for i, vaule := range join2 { //更新join完成
		par.jo.jo[i] = vaule
	}
	//var join3 []Join_each = par.jo.jo
	//for _, vaule := range join3 {
	//	fmt.Println(vaule.tab1 + " " + vaule.tab2 + " " + vaule.join_attr)
	//}
	return par
}

func updateS(par *SQL_query) *SQL_query { //根据table 对select进行排序
	var sels []Select_eachT = make([]Select_eachT, par.tab.size)
	var pos []int = make([]int, par.tab.size)
	for i:=0;i<par.tab.size;i++{
		pos[i]=-1
	}
	for i,sel:=range par.sel.selT{
		for m,tab:=range par.tab.tab{
			if(sel.size>0){
				if(strings.Contains(sel.sel[0].attribute,tab.tab)){
					pos[m]=i
					//pos=append(pos,i)
					break
				}
			}else{
				break
			}
		}
	}
	for j,_:=range pos{
		if(pos[j]!=-1){
			sels[j]=par.sel.selT[pos[j]]
		}
	}
	par.sel = NewSel(sels)
	return par
}

func updateP(par *SQL_query) *SQL_query { //访问GDD，对projection进行排序
	/*
		   访问GDD，某属性是否在某表，
			属性：目标&JOIN
	*/
	//得到所有JOIN的属性j_attr
	var joins []Join_each = par.jo.jo
	var j_attr []string = make([]string, 0)
	for _, value := range joins {
		j_attr = append(j_attr, value.join_attr)
	}
	//得到所有投影的属性p_attr
	var p string = par.pro.pro[0]
	ps:=strings.Split(p,",")
	var p_table []string
	if(par.jo.size==0){
		p_table= make([]string, 1)
		p_table[0] = p
	}else{
		p_table= make([]string, par.jo.size+2)
		p_table[par.jo.size+1] = p
		for i:=0;i<par.jo.size+1;i++ {
			for _,pss:=range ps{
				tname:=strings.Split(pss,".")[0]
				if(tname==par.tab.tab[i].tab){
					p_table[i] =p_table[i]+strings.Split(pss,".")[1]+","
				}
			}
			for _,ja:=range j_attr{
				flag,_:=gddGet.GetTableColumnExist(par.tab.tab[i].tab,ja)
				flag2:=strings.Contains(p_table[i],ja)
				if(flag==1&&!flag2){
					p_table[i] =p_table[i]+ja+","
				}
			}
			p_table[i] = p_table[i][:len(p_table[i])-1]
		}
	}
	par.pro = NewPro(p_table)
	/*
		for _, value := range par.pro.pro {
			fmt.Println(value)
		}*/
	return par
}
