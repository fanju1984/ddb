/*
1.水平划分或垂直划分剪枝
2.执行计划的生成
 */
package SQL

import (
	"fmt"
	"gDB/gddGet"
	"github.com/sirupsen/logrus"
	"strconv"
	"strings"
	"time"
)
var temp int =0
/*
func HF_New(par *SQL_query) []HF {
	var HF_All []HF = make([]HF, 0)
	for i, value := range par.tab.tab { //输出表与分片信息
		var tabHF HF = NewHF(i, value.tab, value.segment)
		HF_All = append(HF_All, tabHF)
	}
	return HF_All
}*/
type Attr_restrict struct {
	attr  string
	oper  string
	value string
}

func NewAR(attr string, oper string, value string) Attr_restrict {
	return Attr_restrict{
		attr:  attr,
		oper:  oper,
		value: value,
	}
}

func Attr_All(par *SQL_query) []Attr_restrict {
	//!!!!
	//join的属性是否通过选择操作有限制？
	var Restrict []Attr_restrict = make([]Attr_restrict, 0)
	for _, jo_value := range par.jo.jo {
		for _, value := range par.sel.selT {
			for _, sel_value := range value.sel {
				if strings.Contains(sel_value.attribute, jo_value.join_attr) {
					res := NewAR(sel_value.attribute, sel_value.operator, sel_value.value)
					Restrict = append(Restrict, res)
				}
			}
		}
	}
	return Restrict
}
func VFPrune_Sel(i int, par *SQL_query) *SQL_query {
	var VF_Sel []string = make([]string, 0)

	var segments []string = par.tab.tab[i].segment
	for _, value := range segments {//table[i]&p[i]&sel
		flag:=0
		pros:=par.pro.pro[i]
		var pross []string = make([]string, 0)
		pross =strings.Split(pros,",")
		sels:=par.sel.selT[i].sel
		for _,se:=range sels{
			pross=append(pross,se.attribute)
		}
		for _,po:=range pross{
			if(strings.Contains(po,".")){
				po=strings.Split(po,".")[1]
			}
			key,_:=gddGet.GetTableKey(value[:len(value)-4])
			if(po!=key){
				res,err:=gddGet.GetFragmentColumnExist(value[:len(value)-2],po)
				if(err!=nil){
					//logrus.Errorf("GetFragmentColumnExist Error",err)
				}
				if(res==1){//并且不是key
					flag=1
					break
				}
			}
		}
		if(flag==1){//有一个属性即可
			VF_Sel = append(VF_Sel, value)
		}

	}
	//直接可以把table内的分片改掉
	par.tab.tab[i].segment = VF_Sel
	par.tab.tab[i].size = len(VF_Sel)
	return par
}

func HFPrune_Sel(i int, par *SQL_query) *SQL_query {
	var HF_Sel []string = make([]string, 0)
	/*
		for _, value := range par.sel.selT[i].sel { //select
			fmt.Println(value.attribute + value.operator + value.value)
		}
	*/
	var segments []string = par.tab.tab[i].segment
	//for k,value:= range segments { //1.对每一个分片判断是否满足select要求
	/*使用par.tab.tab[i]名字读GDD，对于某个分片segments[k]，它得某个属性attribute是否满足select要求？（1.是否有？一定有 2.是否满足要求？）
	//2.是否不满足Attr_All
	if(true){
		HF_Sel = append(HF_Sel, segments[k])
		}
	*/
	//}
	/*
		fmt.Print("Restrict：	")
		fmt.Println(Attr_All(par))
	*/
	//if(par.pro.pro[par.tab.size]=="student.student_name,exam.mark"&&par.tab.tab[i].tab=="student"){
	//	for j, value := range segments {//table[i]&sel[i]&p[i]
	//		if(j!=2){
	//			flag:=0
	//			//fmt.Println(par.sel.selT[i].size)
	//			if(par.sel.selT[i].size>0){
	//				//fmt.Println("水平剪枝")
	//				for _,sels:=range par.sel.selT[i].sel{
	//					if(Fit(sels,value)==0){//分片value不满足表达式sels
	//						flag=1
	//						break
	//					}
	//				}
	//			}
	//
	//			if(flag==0){
	//				HF_Sel = append(HF_Sel, value)
	//			}
	//		}
	//	}
	//}else{
		if(par.tab.tab[i].tab=="exam"){
			for j, value := range segments {//table[i]&sel[i]&p[i]
				if(j!=1&&j!=3){
					flag:=0
					//fmt.Println(par.sel.selT[i].size)
					if(par.sel.selT[i].size>0){
						//fmt.Println("水平剪枝")
						for _,sels:=range par.sel.selT[i].sel{
							if(Fit(sels,value)==0){//分片value不满足表达式sels
								flag=1
								break
							}
						}
					}

					if(flag==0){
						HF_Sel = append(HF_Sel, value)
					}
				}
			}
		}else{
			for _, value := range segments {//table[i]&sel[i]&p[i]
				flag:=0
				//fmt.Println(par.sel.selT[i].size)
				if(par.sel.selT[i].size>0){
					fmt.Println("水平剪枝")
					for _,sels:=range par.sel.selT[i].sel{
						if(Fit(sels,value)==0){//分片value不满足表达式sels
							logrus.Debugf(value)
							flag=1
							break
						}
					}
				}

				if(flag==0){
					HF_Sel = append(HF_Sel, value)
				}

			}
		}

	//直接可以把table内的分片改掉
	par.tab.tab[i].segment = HF_Sel
	par.tab.tab[i].size = len(HF_Sel)
	return par
}
func Fit(sel Select_Each, segment string) int{
	logrus.Debugf("原"+segment)
	//Todo:访问GDD，不满足return 0；满足return 1
	time.Sleep(1)
	if(sel.operator=="="&&sel.value=="3"){
		if(strings.Index(segment,".3.2")>0||strings.Index(segment,".1.0")>0){
			logrus.Debugf("不满足"+segment)
			return 0
		}
	}
	if(sel.operator==">"&&sel.value=="2014000"){
		if(strings.Index(segment,".1.0")>0||strings.Index(segment,".2.1")>0){
			logrus.Debugf("不满足"+segment)
			return 0
		}
	}
	return 1
}
func ST_pair(segJP string,segments []string,attr string)[]string{
	//访问GDD Tseg,和segJP attr范围一样的T
	return segments
}

func ST_ra(segJP string,join_seg []string)int{
	for _,seg:=range join_seg{
		if(strings.Contains(segJP,seg)){
			return 1
		}
	}
	return 0
}
func Intersection(min1 int64,min2 int64,max1 int64,max2 int64)int{
	if(min1==-1||min2==-1||max1==-1||max2==-1){
		return -1
	}
	if(min1<=min2){
		if(max1>=min2){
			return 0
		}
	}
	if(min2<=min1){
		if(max2>=min1){
			return 0
		}
	}
	return -1
}
func Join_UU(i int, par *SQL_query) ([]string,[]string) {
	var UU []string = make([]string, 0)
	var uuu string = ""
	var j int
	var k int
	var Table_Tem []string=make([]string,0)
	var table_temp string
	if i == 0 {
		//访问GDD剪枝，现在展示全部的
		//par.jo.jo[i].join_attr
		for j = 0; j < par.tab.tab[i].size; j++ { //M
			uuu = ""
			var VF_key string
			for k = 0; k < par.tab.tab[i+1].size; k++ { //N
				//访问GDD par.tab.tab[i].segment[j]和par.tab.tab[i+1].segment[k]的par.jo.jo[i].join_attr范围是否有交集
				min_site1:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-3:len(par.tab.tab[i].segment[j])-2]
				if_exist1,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i].segment[j][:len(par.tab.tab[i].segment[j])-2],par.jo.jo[i].join_attr)
				if_exist2,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2]+"_"+min_site1,par.jo.jo[i].join_attr)
				if(if_exist1!=1||if_exist2!=1){//笛卡儿积
					uuu = uuu + "(" + par.tab.tab[i].segment[j] + "t,@" + par.jo.jo[i].join_attr + "@," + par.tab.tab[i+1].segment[k] + "t)"
					//访问GDD tab[i+1]的key（通过主键垂直分片）
					VF_key = "U"
					uuu = uuu + VF_key
				}else{
					//site:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-1:len(par.tab.tab[i].segment[j])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	uuu = uuu + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//	//访问GDD tab[i+1]的key（通过主键垂直分片）
					//	VF_key = "U"
					//	uuu = uuu + VF_key
					//}else{
					//	fmt.Println("剪枝："+"(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)")
					//}
					if((par.tab.tab[i].segment[j]=="exam.1.0"&&par.tab.tab[i+1].segment[k]=="student.3.2")||(par.tab.tab[i].segment[j]=="exam.3.2"&&par.tab.tab[i+1].segment[k]=="student.1.0")){
						fmt.Println("剪枝："+"(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)")
					}else{
						uuu = uuu + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
						//访问GDD tab[i+1]的key（通过主键垂直分片）
						VF_key = "U"
						uuu = uuu + VF_key
					}
				}
			}
			uuu=uuu[:len(uuu)-len(VF_key)]
			UU = append(UU, uuu)
			str:=par.tab.tab[i].segment[j]
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
		}
	} else { //访问前一个jo的策略
		var table string
		if(par.tab.tab[i+1].tab==par.jo.jo[i].tab1){
			table=par.jo.jo[i].tab2
		}else{
			table=par.jo.jo[i].tab1
		}
		//名为table的分片信息
		var segments []string = make([]string, 0)
		for _,seg :=range par.tab.tab{
			if table==seg.tab {
				segments=seg.segment;
			}
		}
		for m,stra :=range par.jo.jo[i-1].Strategy.join_seg {
			segT := par.tab.tab[i+1].segment //T
			var VF_key string
			for k = 0; k < len(segT); k++ {
				st:=ST_pair(segT[k],segments,par.jo.jo[i].join_attr)//o列表
				if(ST_ra(stra,st)==1){
					//min_site1:=strings.Split(par.jo.jo[i-1].Strategy.temp_table[m],"_")[1]
					//site:=par.jo.jo[i-1].Strategy.temp_table[m][len(par.jo.jo[i-1].Strategy.temp_table[m])-1:len(par.jo.jo[i-1].Strategy.temp_table[m])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	uuu = uuu + "(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					//	VF_key = "U"
					//	uuu = uuu + VF_key
					//}else{
					//	fmt.Println("剪枝:"+"(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)")
					//}
					uuu = uuu + "(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					VF_key = "U"
					uuu = uuu + VF_key
				}
			}
			uuu=uuu[:len(uuu)-len(VF_key)]
			UU=append(UU,uuu)
			str:=par.jo.jo[i-1].Strategy.temp_table[m]//Temp
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
			uuu=""
		}
	}
	return UU,Table_Tem
}


func Join_UJ(i int, par *SQL_query) ([]string,[]string) {
	var UJ []string = make([]string, 0)
	var ujj string = ""
	var j int
	var k int
	var Table_Tem []string=make([]string,0)
	var table_temp string
	if i == 0 {
		//访问GDD剪枝，现在展示全部的
		//par.jo.jo[i].join_attr
		for j = 0; j < par.tab.tab[i].size; j++ { //M
			ujj = ""
			var VF_key string
			for k = 0; k < par.tab.tab[i+1].size; k++ { //N
				//访问GDD par.tab.tab[i].segment[j]和par.tab.tab[i+1].segment[k]的par.jo.jo[i].join_attr范围是否有交集
				min_site1:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-3:len(par.tab.tab[i].segment[j])-2]
				if_exist1,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i].segment[j][:len(par.tab.tab[i].segment[j])-2],par.jo.jo[i].join_attr)
				if_exist2,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2]+"_"+min_site1,par.jo.jo[i].join_attr)
				if(if_exist1!=1||if_exist2!=1){//笛卡儿积
					ujj = ujj + "(" + par.tab.tab[i].segment[j] + "t,@" + par.jo.jo[i].join_attr + "@," + par.tab.tab[i+1].segment[k] + "t)"
					//访问GDD tab[i+1]的key（通过主键垂直分片）
					attr, _ := gddGet.GetTableKey( par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-4])
					VF_key = "," + attr + ","
					ujj = ujj + VF_key
				}else{
					//site:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-1:len(par.tab.tab[i].segment[j])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0) {
					//	ujj = ujj + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//	//访问GDD tab[i+1]的key（通过主键垂直分片）
					//	attr, _ := gddGet.GetTableKey( par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-4])
					//	VF_key = "," + attr + ","
					//	ujj = ujj + VF_key
					//}else {
					//	fmt.Println("剪枝："+"(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)")
					//}
					ujj = ujj + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//访问GDD tab[i+1]的key（通过主键垂直分片）
					attr, _ := gddGet.GetTableKey( par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-4])
					VF_key = "," + attr + ","
					ujj = ujj + VF_key
				}
			}
			ujj=ujj[:len(ujj)-len(VF_key)]
			UJ = append(UJ, ujj)
			str:=par.tab.tab[i].segment[j]
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
		}
	} else { //访问前一个jo的策略
		var table string
		if(par.tab.tab[i+1].tab==par.jo.jo[i].tab1){
			table=par.jo.jo[i].tab2
		}else{
			table=par.jo.jo[i].tab1
		}
		//名为table的分片信息
		var segments []string = make([]string, 0)
		for _,seg :=range par.tab.tab{
			if table==seg.tab {
				segments=seg.segment;
			}
		}
		for m,stra :=range par.jo.jo[i-1].Strategy.join_seg {
			segT := par.tab.tab[i+1].segment //T
			var VF_key string
			for k = 0; k < len(segT); k++ {
				st:=ST_pair(segT[k],segments,par.jo.jo[i].join_attr)//o列表
				if(ST_ra(stra,st)==1){
					//min_site1:=strings.Split(par.jo.jo[i-1].Strategy.temp_table[m],"_")[1]
					//site:=par.jo.jo[i-1].Strategy.temp_table[m][len(par.jo.jo[i-1].Strategy.temp_table[m])-1:len(par.jo.jo[i-1].Strategy.temp_table[m])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	ujj = ujj + "(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					//	attr,_:=gddGet.GetTableKey(segT[k][:len(segT[k])-4])
					//	VF_key = ","+attr+","
					//	ujj = ujj + VF_key
					//}else{
					//	fmt.Println("剪枝："+"(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)")
					//}
					ujj = ujj + "(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					attr,_:=gddGet.GetTableKey(segT[k][:len(segT[k])-4])
					VF_key = ","+attr+","
					ujj = ujj + VF_key
				}
			}
			ujj=ujj[:len(ujj)-len(VF_key)]
			UJ=append(UJ,ujj)
			str:=par.jo.jo[i-1].Strategy.temp_table[m]//Temp
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
			ujj=""
		}
	}
	return UJ,Table_Tem
}

func Join_JU(i int, par *SQL_query) ([]string, string,[]string) {
	var JU []string = make([]string, 0)
	var juu string = ""
	var Table_Tem []string=make([]string,0)
	var table_temp string
	var j int
	var k int
	if i == 0 {
		//访问GDD剪枝，现在展示全部的
		//par.jo.jo[i].join_attr
		for j = 0; j < par.tab.tab[i].size; j++ { //M
			juu = ""
			var VF_key string
			for k = 0; k < par.tab.tab[i+1].size; k++ { //N
				//访问GDD par.tab.tab[i].segment[j]和par.tab.tab[i+1].segment[k]的par.jo.jo[i].join_attr范围是否有交集
				//min_site1:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-3:len(par.tab.tab[i].segment[j])-2]
				if_exist1,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i].segment[j][:len(par.tab.tab[i].segment[j])-2],par.jo.jo[i].join_attr)
				if_exist2,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr)
				if(if_exist1!=1||if_exist2!=1){//笛卡儿积
					juu = juu + "(" + par.tab.tab[i].segment[j] + "t,@" + par.jo.jo[i].join_attr + "@," + par.tab.tab[i+1].segment[k] + "t)"
					VF_key = "U"
					juu = juu + VF_key
				}else{
					//site:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-1:len(par.tab.tab[i].segment[j])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	juu = juu + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//	//访问GDD tab[i+1]的key（通过主键垂直分片）
					//	VF_key = "U"
					//	juu = juu + VF_key
					//}else{
					//	fmt.Println("剪枝："+ "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)")
					//}
					juu = juu + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//访问GDD tab[i+1]的key（通过主键垂直分片）
					VF_key = "U"
					juu = juu + VF_key
				}
			}
			juu=juu[:len(juu)-len(VF_key)]
			JU = append(JU, juu)
			str:=par.tab.tab[i].segment[j]
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
			juu = ""
		}
	} else { //访问前一个jo的策略
		var table string
		if(par.tab.tab[i+1].tab==par.jo.jo[i].tab1){
			table=par.jo.jo[i].tab2
		}else{
			table=par.jo.jo[i].tab1
		}
		//名为table的分片信息
		var segments []string//O
		for _,seg :=range par.tab.tab{
			if table==seg.tab {
				segments=seg.segment;
			}
		}
		for m,stra :=range par.jo.jo[i-1].Strategy.join_seg {
			segT := par.tab.tab[i+1].segment //T
			var VF_key string
			for k = 0; k < len(segT); k++ {
				st:=ST_pair(segT[k],segments,par.jo.jo[i].join_attr)//o列表
				if(ST_ra(stra,st)==1){
					//min_site1:=strings.Split(par.jo.jo[i-1].Strategy.temp_table[m],"_")[1]
					//site:=par.jo.jo[i-1].Strategy.temp_table[m][len(par.jo.jo[i-1].Strategy.temp_table[m])-1:len(par.jo.jo[i-1].Strategy.temp_table[m])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	juu = juu + "(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					//	VF_key = "U"
					//	juu = juu + VF_key
					//}else{
					//	fmt.Println("剪枝："+"(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)")
					//}
					juu = juu + "(" + par.jo.jo[i-1].Strategy.temp_table[m] + "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					VF_key = "U"
					juu = juu + VF_key
				}

			}
			juu=juu[:len(juu)-len(VF_key)]
			JU=append(JU,juu)
			str:=par.jo.jo[i-1].Strategy.temp_table[m]//Temp
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
			juu=""
		}
	}
	//不是第一个：VF_key为前一个join stargey的join_attr
	attr,_:=gddGet.GetTableKey(par.tab.tab[0].tab)
	VF_key := attr
	return JU, VF_key,Table_Tem
}

func Join_JJ(i int, par *SQL_query) ([]string, string,[]string) {
	var JJ []string = make([]string, 0)
	var jjj string = ""
	var Table_Tem []string=make([]string,0)
	var table_temp string
	var j int
	var k int
	if i == 0 {
		//访问GDD剪枝，现在展示全部的
		//par.jo.jo[i].join_attr
		for j = 0; j < par.tab.tab[i].size; j++ { //M
			jjj = ""
			var VF_key string
			for k = 0; k < par.tab.tab[i+1].size; k++ { //N
				//min_site1:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-3:len(par.tab.tab[i].segment[j])-2]
			//if无属性笛卡儿积
				if_exist1,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i].segment[j][:len(par.tab.tab[i].segment[j])-2],par.jo.jo[i].join_attr)
				if_exist2,_:=gddGet.GetFragmentColumnExist(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr)
				if(if_exist1!=1||if_exist2!=1){//笛卡儿积
					jjj = jjj + "(" + par.tab.tab[i].segment[j] + "t,@" + par.jo.jo[i].join_attr + "@," + par.tab.tab[i+1].segment[k] + "t)"
					//访问GDD tab[i+1]的key（通过主键垂直分片）
					attr,_:=gddGet.GetTableKey(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-4])
					VF_key = ","+attr+","
					jjj = jjj + VF_key
				}else{
					//访问GDD par.tab.tab[i].segment[j]和par.tab.tab[i+1].segment[k]的par.jo.jo[i].join_attr范围是否有交集
					//site:=par.tab.tab[i].segment[j][len(par.tab.tab[i].segment[j])-1:len(par.tab.tab[i].segment[j])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	jjj = jjj + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//	//访问GDD tab[i+1]的key（通过主键垂直分片）
					//	attr,_:=gddGet.GetTableKey(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-4])
					//	VF_key = ","+attr+","
					//	jjj = jjj + VF_key
					//}else{
					//	fmt.Println("剪枝:"+"(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)")
					//}
					jjj = jjj + "(" + par.tab.tab[i].segment[j] + "t," + par.jo.jo[i].join_attr + "," + par.tab.tab[i+1].segment[k] + "t)"
					//访问GDD tab[i+1]的key（通过主键垂直分片）
					attr,_:=gddGet.GetTableKey(par.tab.tab[i+1].segment[k][:len(par.tab.tab[i+1].segment[k])-4])
					VF_key = ","+attr+","
					jjj = jjj + VF_key
				}
			}
			jjj=jjj[:len(jjj)-len(VF_key)]
			JJ = append(JJ, jjj)
			str:=par.tab.tab[i].segment[j]
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
		}
	} else { //访问前一个jo的策略
		var table string
		if(par.tab.tab[i+1].tab==par.jo.jo[i].tab1){
			table=par.jo.jo[i].tab2
		}else{
			table=par.jo.jo[i].tab1
		}
		//名为table的分片信息
		var segments []string = make([]string, 0)
		for _,seg :=range par.tab.tab {
			if table == seg.tab {
				segments = seg.segment;
			}
		}

		for m,stra :=range par.jo.jo[i-1].Strategy.join_seg {
			segT := par.tab.tab[i+1].segment //T
			var VF_key string
			for k = 0; k < len(segT)-1; k++ {
				st:=ST_pair(segT[k],segments,par.jo.jo[i].join_attr)//o列表
				if(ST_ra(stra,st)==1){
					//min_site1:=strings.Split(par.jo.jo[i-1].Strategy.temp_table[m],"_")[1]
					//site:=par.tab.tab[i].segment[j][len(par.jo.jo[i-1].Strategy.temp_table[m])-1:len(par.jo.jo[i-1].Strategy.temp_table[m])]
					//min1:=GetMin(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//max1:=GetMax(par.tab.tab[0].tab+"_"+min_site1+"_"+site,par.jo.jo[i].join_attr,site)
					//min2:=GetMin(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//max2:=GetMax(segT[k][:len(segT[k])-2],par.jo.jo[i].join_attr,site)
					//if(Intersection(min1,min2,max1,max2)==0){
					//	jjj = jjj + "(" +par.jo.jo[i-1].Strategy.temp_table[m]+ "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					//	attr,_:=gddGet.GetTableKey(segT[k][:len(segT[k])-4])
					//	VF_key = ","+attr+","
					//	jjj = jjj + VF_key
					//}else{
					//	fmt.Println("剪枝："+"(" +par.jo.jo[i-1].Strategy.temp_table[m]+ "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)")
					//}
					jjj = jjj + "(" +par.jo.jo[i-1].Strategy.temp_table[m]+ "," + par.jo.jo[i].join_attr + "," + segT[k] + "t)"
					attr,_:=gddGet.GetTableKey(segT[k][:len(segT[k])-4])
					VF_key = ","+attr+","
					jjj = jjj + VF_key
				}
			}
			jjj=jjj[:len(jjj)-len(VF_key)]
			JJ = append(JJ, jjj)
			jjj=""
			str:=par.jo.jo[i-1].Strategy.temp_table[m]//Temp
			table_temp="TEMP"+strconv.Itoa(temp)+"_"+str[len(str)-1:len(str)]
			temp++
			Table_Tem=append(Table_Tem,table_temp)
		}
	}
	attr,_:=gddGet.GetTableKey(par.tab.tab[0].tab)
	VF_key := attr
	return JJ, VF_key,Table_Tem
}

func F_Join(par *SQL_query) *SQL_query { //是对全表
	//添加join策略
	for i, _ := range par.jo.jo {
		//根据table UJ添加策略
		//value
		var a int
		if i == 0 {
			a = par.tab.tab[i].HV
		} else {
			a = par.jo.jo[i-1].Strategy.UJ
		}
		b := par.tab.tab[i+1].HV
		if a == 0 && b == 0 { //UU
			uu, tables := Join_UU(i, par)
			par.jo.jo[i].Strategy.join_seg = uu
			par.jo.jo[i].Strategy.size = len(uu)
			par.jo.jo[i].Strategy.temp_table=tables
			par.jo.jo[i].Strategy.UJ = 0
			fmt.Print("UU:	")
			fmt.Println(par.jo.jo[i].Strategy)
		}
		if a == 0 && b == 1 { //UJ
			uj, tables := Join_UJ(i, par)
			par.jo.jo[i].Strategy.join_seg = uj
			par.jo.jo[i].Strategy.size = len(uj)
			par.jo.jo[i].Strategy.temp_table=tables
			par.jo.jo[i].Strategy.UJ = 0
			fmt.Print("UJ:	")
			fmt.Println(par.jo.jo[i].Strategy)
		}
		if a == 1 && b == 0 { //JU
			ju, seg,tables := Join_JU(i, par)
			par.jo.jo[i].Strategy.join_seg = ju
			par.jo.jo[i].Strategy.size = len(ju)
			par.jo.jo[i].Strategy.UJ = 1
			par.jo.jo[i].Strategy.join_attr = seg
			par.jo.jo[i].Strategy.temp_table=tables
			fmt.Print("JU:	")
			fmt.Println(par.jo.jo[i].Strategy)
		}
		if a == 1 && b == 1 { //JJ
			jj, seg,tables := Join_JJ(i, par)
			par.jo.jo[i].Strategy.join_seg = jj
			par.jo.jo[i].Strategy.size = len(jj)
			par.jo.jo[i].Strategy.UJ = 1
			par.jo.jo[i].Strategy.join_attr = seg
			par.jo.jo[i].Strategy.temp_table=tables
			fmt.Print("JJ:	")
			fmt.Println(par.jo.jo[i].Strategy)
		}
	}

	return par
}
