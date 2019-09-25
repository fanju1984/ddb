/*
执行计划的执行
1.为每一个站点开启线程执行plan
2.将执行计划转换为对应站点的plan，方便站点按照plan串行执行而且各个站点之间并行
 */
package SQL

import (
	"context"
	"fmt"
	"gDB/gddGet"
	"gDB/pb"
	log "github.com/sirupsen/logrus"
	"regexp"
	"strconv"
	"strings"
	"gDB/storage"
)
type Tab_Send struct{
	name string
	turn int
	to []string
	jo []string
}
var FLAG_CROSS int=0

var T int=0

type Plan_S struct{
	step_size int
	step_plan []string
}

type Plan struct{
	site_max int
	plans []Plan_S
}

type Site_Send struct{
	tab []Tab_Send
	len int
}

type Join_Element struct{
	site int
	sql []string
	UJ int
	join string
}

type Join_Element_turn struct{
	joinE []Join_Element
}

func Send(from int, to []int,sql string,plan Plan){
	fmt.Print("site_from")
	fmt.Println(from)
	targetSite,err1:=storage.Storage.GetRPCClient("st"+strconv.Itoa(from))
	if err1 != nil {
		log.Errorf("targetSite1失败 is %v", err1)
		fmt.Println("targetSite1失败")
	}
	var q peerCommunication.SelectQuery
	q.Query=sql
	fmt.Println(sql)
	var qp string=""
	for i:=0;i<plan.site_max;i++{
		if(plan.plans[i].step_size>0){
			qp=qp+"<"+strconv.Itoa(i)+">"
			m:=0
			for m=0;m<len(plan.plans[i].step_plan)-1;m++{
				qp=qp+plan.plans[i].step_plan[m]+"&"
			}
			qp=qp+plan.plans[i].step_plan[m]+"|"
		}else{
			qp=qp+"<"+strconv.Itoa(i)+">nil|"
		}
	}
	q.Query =q.Query+"!!"+qp
	q.ToSite=""
	for _,t:=range to{
		q.ToSite=q.ToSite+"st"+strconv.Itoa(t)+","
	}
	q.ToSite=q.ToSite[:len(q.ToSite)-1]
	state,error:=targetSite.Client.Select(context.Background(),&q)
	if(error!=nil){
		log.Errorf("error_5失败 is %v", error)
		fmt.Print("error_5")
	}
	if(state.Ack=="Select Start"){
		fmt.Println("Select Start")
	}
}
func TEMP_Create(Site int,sql string,tab string,res_plan Plan) Plan{
	fmt.Print(Site)
	fmt.Print(" : ")
	fmt.Println("drop view if exists "+tab+";CREATE VIEW "+tab+" AS "+sql)
	Query:="drop view if exists "+tab+";CREATE VIEW "+tab+" AS "+sql
	res_plan.plans[Site].step_size++
	res_plan.plans[Site].step_plan=append(res_plan.plans[Site].step_plan,Query)
	return res_plan
}


func Send_single(site int, sql []string, table[]string){//本地create选择投影后的table
	fmt.Print("Site: st")
	fmt.Println(site)
	targetSite,err3:=storage.Storage.GetRPCClient("st"+strconv.Itoa(site))
	if err3 != nil {
		log.Errorf("error_targetSite3 is %v", err3)
		fmt.Println("targetSite3失败")
	}
	var q peerCommunication.Query
	for k:=0;k<len(sql);k++{
		tab:=strings.Replace(table[k],".","_",-1)
		sql1:=strings.Replace(sql[k],".","_",-1)
		q.Query="[VIEW]drop view if exists "+tab+";CREATE VIEW "+tab+" AS "+sql1
		fmt.Println(q.Query)
		state,error:=targetSite.Client.Execute(context.Background(),&q)
		if(error!=nil){
			log.Errorf("error_4 is %v", error)
			fmt.Print("error_4")
		}
		if(state.EffectedLine==1){
			fmt.Println("CreateView_4 success")
		}
	}
}

func Reg_Str(strategy_each string)[]string{
	stra:=strings.Split(strategy_each,",")
	var Res []string = make([]string, 0)
	Res=append(Res,strings.Split(stra[0],"(")[1])
	Res=append(Res,strings.Split(stra[2],")")[0])
	return Res
}
func Site_Size(par *SQL_query) int{
	max:=0
	for _,table:=range par.tab.tab{
		site:=table.segment[len(table.segment)-1]
		site1,_ := strconv.Atoi(site[len(site)-1:len(site)])
		if(max<site1){
			max=site1
		}
	}
	return max+1
}
func Plan_Form(par *SQL_query) Plan{
	max:=Site_Size(par)
	var res_plan Plan
	res_plan.site_max=max
	res_plan.plans=make([]Plan_S,max)
	for i:=0;i<max;i++{
		res_plan.plans[i].step_size=0
		res_plan.plans[i].step_plan=make([]string,0)
	}
	Site := make([]Site_Send, max)
	for _,site_e:=range Site{
		site_e.len=0
	}
	//str:=",(.)*?\\)"
	//exp_p := regexp.MustCompile(str)

	//max:=Site_Size(par)
	turn:=-1
	flag:=0
	if(par.jo.size==0){
		for _,tab:=range par.tab.tab{
			flag=tab.HV
			//fmt.Println("-----------------step5: table transfer-----------------")
			////默认发给站点0
			//fmt.Println("site_to: 0")
			//for _,seg:=range tab.segment{
			//	//pos,_:=strconv.Atoi(seg[len(seg)-1:len(seg)])
			//
			//		//if T为空（H）
			//		//if 无目标属性(V)
			//		send_from:="site_from"+seg[len(seg)-1:len(seg)]
			//		seg2:=strings.Replace(seg,".","_",-1)
			//		send:="select * from "+seg2+"t"
			//		fmt.Println(send_from+" : "+send)
			//		targetSite,err5:=storage.Storage.GetRPCClient("st"+seg[len(seg)-1:len(seg)])
			//		if err5 != nil {
			//			log.Errorf("error_5 is %v", err5)
			//			fmt.Println("targetSite5失败")
			//		}
			//		var q peerCommunication.SelectQuery
			//		q.Query=send
			//		q.ToSite=",st0"
			//		state,error:=targetSite.Client.Select(context.Background(),&q)
			//		if(error!=nil){
			//			log.Errorf("error_5_s is %v", error)
			//			fmt.Println("error_5_s")
			//		}
			//		if(state.Ack=="Select Start"){
			//			fmt.Println("Select Start")
			//		}
			//}
			fmt.Println("-----------------step6: last join/union-----------------")
			fmt.Println("site_to: 0")
			Q:="[EXIST]select exists (select 1 from "
			if(flag==0){//H
				//if T为空（H）已在HFPrune_Sel剪枝
				stra:=""
				for _,tab:=range par.tab.tab{
					for _,seg:=range tab.segment{
						tab:=strings.Replace(seg,".","_",-1)
						//stra=stra+"select * from "+tab+"t"+" union all "
						stra=stra+"select * from "+tab+"t"+" union all "
						Q=Q+tab+"t,"
					}
				}
				Q=Q[:len(Q)-1]+")"
				res_plan.plans[0].step_size++
				res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,Q)
				fmt.Println(stra[:len(stra)-7])
				Query:="[FINAL]"+stra[:len(stra)-7]
				res_plan.plans[0].step_size++
				res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,Query)

			}else{//V
				//if 无目标属性(V) 已在VFPrune_Sel剪枝
				stra:="select * from "
				Q:="[EXIST]select exists (select 1 from "
				for _,tab:=range par.tab.tab{
					for _,seg:=range tab.segment{
						tab:=strings.Replace(seg,".","_",-1)
						stra=stra+tab+"t"+", "
						Q=Q+tab+"t,"
					}
				}
				//if(len(table)==1)
				//fmt.Println(stra[:len(stra)-2])
				Q=Q[:len(Q)-1]+")"
				res_plan.plans[0].step_size++
				res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,Q)
				stra=stra[:len(stra)-2]+" where "
				for _,tab:=range par.tab.tab{
					for i:=0;i<len(tab.segment)-1;i++ {
						key,err0:=gddGet.GetTableKey(tab.tab)
						if err0 != nil {
							log.Errorf("GetTableKey0失败 is %v", err0)
							fmt.Println("GetTableKey0失败")
						}
						tab1:=strings.Replace(tab.segment[i],".","_",-1)
						tab2:=strings.Replace(tab.segment[i+1],".","_",-1)
						stra=stra+tab1+"t."+key+" = "+tab2+"t."+key+" and "
					}
				}
				fmt.Println(stra[:len(stra)-5])
				Query:="[FINAL]"+stra[:len(stra)-5]
				res_plan.plans[0].step_size++
				res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,Query)
			}
		}
		return res_plan
	}
	for _,jo:=range par.jo.jo{//多个join
		//fmt.Println(jo.Strategy)
		turn++
		for _,strategy_each:=range jo.Strategy.join_seg{//每一个join
			//fmt.Println(i)
			//fmt.Println(strategy_each)
			var tabs1 []string = make([]string, 0)
			var tabs2 []string = make([]string, 0)
			tab1:=strings.Split(strategy_each,"(")
			for k:=0;k<2;k++{
				tab_e:=strings.Split(tab1[k],",")
				tabs1=append(tabs1,tab_e[0])
			}
			//fmt.Print(tabs1[1])
			//fmt.Println("!!!!!!!!!!!!!!!")
			tab2:=strings.Split(strategy_each,")")
			for k:=0;k<len(tab2)-1;k++{
				tab_e:=strings.Split(tab2[k],",")
				tabs2=append(tabs2,tab_e[len(tab_e)-1])
			}
			//fmt.Print(tabs2)
			//fmt.Println("!!!!!!!!!!!!!!!")
			exp_p := regexp.MustCompile( ",(.)*?,")
			all := exp_p.FindAllString(strategy_each, -1)
			jo:=strings.Replace(all[0],",","",-1)
			//fmt.Println(jo)

			for _,tabe:=range tabs2{
				site,_:=strconv.Atoi(tabe[len(tabe)-2:len(tabe)-1])
				var tab Tab_Send
				flag:=0

				for j:=0;j<Site[site].len;j++{
					if(Site[site].tab[j].name==tabe){
						Site[site].tab[j].to=append(Site[site].tab[j].to, tabs1[1])
						Site[site].tab[j].jo=append(Site[site].tab[j].jo, jo)
						flag=1
					}
				}
				if(flag==0){
					tab.jo=append(tab.jo,jo)
					tab.name=tabe
					tab.turn=turn
					tab.to=append(tab.to, tabs1[1])
					Site[site].tab=append(Site[site].tab, tab)
					Site[site].len++;
				}

			}
		}

	}
	//fmt.Println(Site)
	//fmt.Println("-----------------step5: table transfer-----------------")

	var joinTurns []Join_Element_turn=make([]Join_Element_turn,0)
	for j:=0;j<par.jo.size;j++{//turn
		fmt.Print("turn")
		fmt.Println(j)
		var  joinTurn Join_Element_turn
		for _,site_e:=range Site {
			for _, tab := range site_e.tab {
				if (tab.turn == j) {
					for i, to := range tab.to {
						var site_to int
						var str string
						Q:="[EXIST]select exists (select 1 from "
						if (tab.turn == 0) {
							site_to, _ = strconv.Atoi(to[len(to)-2 : len(to)-1])
							name:=strings.Replace(tab.name,".","_",-1)
							to_name:=strings.Replace(to,".","_",-1)
							Q=Q+name+","+to_name+")"
							if(strings.Contains(tab.jo[i],"@")){
								FLAG_CROSS=1
								str = "select * from " + name + " cross join " + to_name
							}else{
								str = "select * from " + name + " inner join " + to_name + " using(" +tab.jo[i]+")"
								//str = "select * from " + name + "," + to_name + " where " + name + "." + tab.jo[i] + "=" + to_name + "." + tab.jo[i]
							}
						} else {
							//访问GDD，TEMP表在哪个位置呢！！
							site_to, _ = strconv.Atoi(to[len(to)-1 : len(to)])
							name:=strings.Replace(tab.name,".","_",-1)
							to_name:=strings.Replace(to,".","_",-1)
							Q=Q+name+","+to_name+")"
							if(strings.Contains(tab.jo[i],"@")){
								FLAG_CROSS=1
								str = "select * from " + name + " cross join " + to_name
							}else{
								str = "select * from " + name + " inner join " + to_name + " using(" +tab.jo[i]+")"
							}
						}
						res_plan.plans[site_to].step_size++
						res_plan.plans[site_to].step_plan=append(res_plan.plans[site_to].step_plan,Q)
						//fmt.Print(site_to)
						//fmt.Print(" : ")
						//fmt.Println(str)
						flagj := 0
						if (len(joinTurn.joinE) == 0) {
							joinTurn.joinE = make([]Join_Element, 1)
							joinTurn.joinE[0].site = site_to
							joinTurn.joinE[0].sql=make([]string,1)
							joinTurn.joinE[0].sql[0] = str
						}else{
							for i, jj := range joinTurn.joinE {
								if (site_to == jj.site) {
									flagj = 1
									joinTurn.joinE[i].sql= append(joinTurn.joinE[i].sql,str)
									break
								}
							}
							if (flagj == 0) {
								var je Join_Element
								je.site=site_to
								je.sql=make([]string,1)
								je.sql[0]=str
								joinTurn.joinE = append(joinTurn.joinE, je)
							}
						}

					}
				}
			}
		}


		//joinTurn.joinE[]//tos定
		joinTurns=append(joinTurns,joinTurn)
		//一个turn send完成，执行操作！！！

	}
	fmt.Println("-----------------step6: table join-----------------")
	//fmt.Println(joinTurns)
	for i,turn:=range joinTurns{
		fmt.Print("turn")
		fmt.Print(i+1)
		fmt.Println(" : ")
		for k,m:=range turn.joinE{
			fmt.Print("site")
			fmt.Println(m.site)
			var Table []string =make([]string,0)
			for _,sql:=range m.sql{
				//fmt.Print(sql)
				//Table= append(Table,"t"+strconv.Itoa(i) )
				Table=append(Table,"t"+strconv.Itoa(T) )
				fmt.Println("drop view if exists "+"t"+strconv.Itoa(T)+";CREATE VIEW "+"t"+strconv.Itoa(T)+" AS "+sql)
				Query:="drop view if exists "+"t"+strconv.Itoa(T)+";CREATE VIEW "+"t"+strconv.Itoa(T)+" AS "+sql
				T++
				res_plan.plans[m.site].step_size++
				res_plan.plans[m.site].step_plan=append(res_plan.plans[m.site].step_plan,Query)
			}
			//在m.site站点执行对m.sql内所有临时表的U或者join
			//Temp生成
			var sql string
			var attr0 string
			Q:="[EXIST]select exists (select 1 from "
			sql="select * from "
			var j int
			for j=0;j<len(Table)-1;j++{
				name:=strings.Replace(Table[j],".","_",-1)
				sql=sql+name+","
				Q=Q+name+","
			}
			Q=Q+Table[j]
			sql=sql+Table[j]+" where "
			if(strings.Contains(par.jo.jo[i].Strategy.join_seg[0],"),")){
				if(strings.Contains(par.jo.jo[i].Strategy.join_seg[0],"@")){//笛卡儿积
					sql:=sql[:len(sql)-7]
					sql2:=strings.Split(sql,"from ")[1]
					tabs:=strings.Split(sql2,",")
					sql="select * from "
					for _,ta:=range tabs{
						sql=sql+ta+" cross join "
					}
					sql=sql[:len(sql)-12]
					Q=Q+")"

				}else{
					sql="select * from "
					attr:=strings.Split(par.jo.jo[i].Strategy.join_seg[0],"),")
					attr1:=strings.Split(attr[1],",(")
					attr0=attr1[0]
					for j=0;j<len(Table)-2;j++{
						name1:=strings.Replace(Table[j],".","_",-1)
						name2:=strings.Replace(Table[j+1],".","_",-1)
						sql=sql+name1+" inner join "+name2+" using("+attr0+")"+" and "
						//sql=sql+name1+"."+attr0+"="+name2+"."+attr0+" and "
					}
					if(i==1){
						p:="student_id,student_name,mark,course_id"
						name1:=strings.Replace(Table[j],".","_",-1)
						name2:=strings.Replace(Table[j+1],".","_",-1)
						sql=sql+name1+" inner join "+name2+" using("+p+")"
					}else{
						name1:=strings.Replace(Table[j],".","_",-1)
						name2:=strings.Replace(Table[j+1],".","_",-1)
						sql=sql+name1+" inner join "+name2+" using("+attr0+")"
						Q=Q+")"
					}
				}

			}else{
				sql=""
				for j=0;j<len(Table)-1;j++{
					name:=strings.Replace(Table[j],".","_",-1)
					if(FLAG_CROSS==1){
						sql=sql+"select * from "+name+" union "
					}else {
						sql = sql + "select * from " + name + " union all "
					}
					//sql=sql+"select * from "+name+" union all "
					//}
					//sql=sql+"select * from "+name+" union "
					//Q=Q+name+","
				}
				name:=strings.Replace(Table[j],".","_",-1)
				sql=sql+"select * from "+name
				Q=Q+")"
			}
			res_plan.plans[m.site].step_size++
			res_plan.plans[m.site].step_plan=append(res_plan.plans[m.site].step_plan,Q)
			res_plan=TEMP_Create(m.site,sql,par.jo.jo[i].Strategy.temp_table[k],res_plan)
			//马上清空T表
			fmt.Println(Q)
			fmt.Println("Clearing T")
		}
	}
	fmt.Println("-----------------step7: LAST-----------------")
	//假设传给site1
	fmt.Println("-----------------step7.1: table transfer-----------------")
	fmt.Println("site_to : 0")
	for _,tab:=range par.jo.jo[par.jo.size-1].Strategy.temp_table{
		fmt.Print("site_from"+tab[len(tab)-1:len(tab)]+" : ")
		name:=strings.Replace(tab,".","_",-1)
		Q:="[EXIST]select exists (select 1 from "
		Q=Q+name+")"
		sql:="[sent to 0]"+"$"+name+"$"+"select * from "+name
		fmt.Println(sql)
		Query:=sql
		site,_:=strconv.Atoi(tab[len(tab)-1:len(tab)])
		res_plan.plans[site].step_size++
		res_plan.plans[site].step_plan=append(res_plan.plans[site].step_plan,Q)
		res_plan.plans[site].step_size++
		res_plan.plans[site].step_plan=append(res_plan.plans[site].step_plan,Query)
	}
	fmt.Println("-----------------step7.2: last join/union-----------------")
	var sql string
	if_sql:=""
	if(par.jo.jo[par.jo.size-1].Strategy.UJ==1){
		ts:=par.pro.pro[par.tab.size]
		tss:=strings.Split(ts,",")
		sql="select "
		for _,s:=range tss{
			if(strings.Contains(s,".")){
				s =strings.Split(s,".")[1]
				sql=sql+s+","
			}
		}
		sql=sql[:len(sql)-1]
		sql=sql+" from "
		var j int
		var attr0 string=par.jo.jo[par.jo.size-1].Strategy.join_attr
		for j=0;j<len(par.jo.jo[par.jo.size-1].Strategy.temp_table)-2;j++{
			name1:=strings.Replace(par.jo.jo[par.jo.size-1].Strategy.temp_table[j],".","_",-1)
			name2:=strings.Replace(par.jo.jo[par.jo.size-1].Strategy.temp_table[j+1],".","_",-1)
			//sql=sql+name1+"."+attr0+"="+name2+"."+attr0+" and "
			sql=sql+name1+" inner join "+name2+" inner join "
		}
		name1:=strings.Replace(par.jo.jo[par.jo.size-1].Strategy.temp_table[j],".","_",-1)
		name2:=strings.Replace(par.jo.jo[par.jo.size-1].Strategy.temp_table[j+1],".","_",-1)
		sql=sql+name1+" inner join "+name2+" using("+attr0+")"
		Q:="[EXIST]select exists (select 1 from "+if_sql+")"
		res_plan.plans[0].step_size++
		res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,Q)
	}else{
		ts:=par.pro.pro[par.tab.size]
		tss:=strings.Split(ts,",")
		var pos string=""
		for _,s:=range tss{
			if(strings.Contains(s,".")){
				s =strings.Split(s,".")[1]
				pos=pos+s+","
			}
		}
		pos=pos[:len(pos)-1]
		if_sql="[EXIST]select exists (select 1 from "
		sql="select "+pos+" from "
		var m int
		for m=0;m<len(par.jo.jo[par.jo.size-1].Strategy.temp_table)-1;m++{
			tab:=strings.Replace(par.jo.jo[par.jo.size-1].Strategy.temp_table[m],".","_",-1)
			sql=sql+tab+" union all select "+pos+" from "
			if_sql=if_sql+tab+","
		}
		tab:=strings.Replace(par.jo.jo[par.jo.size-1].Strategy.temp_table[m],".","_",-1)
		sql=sql+tab
		if_sql=if_sql+tab+")"
		res_plan.plans[0].step_size++
		res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,if_sql)
	}
	sql=sql
	fmt.Println(sql)
	Query:="[FINAL]"+sql
	res_plan.plans[0].step_size++
	res_plan.plans[0].step_plan=append(res_plan.plans[0].step_plan,Query)
	return res_plan
}
func Excute_sin(par *SQL_query) {
	seg:=par.tab.tab[0].segment[0]
	seg=strings.Replace(seg,".","_",-1)
	var site string
	site =seg[len(seg)-1:len(seg)]
	seg=seg+"t"
	var q peerCommunication.Query
	sql:="[SIN]select * from "+seg
	q.Query=sql
	targetSite, err5 := storage.Storage.GetRPCClient("st" + site)
	if(err5!=nil){
		log.Println("Excute_sin err %v",err5)
	}
	state, err := targetSite.Client.Execute(context.Background(), &q)
	if (err != nil) {
		log.Errorf("error_5_sin is %v", err)
		fmt.Println("error_5_sin")
	}
	if(state.EffectedLine!=10){
		fmt.Print("Select Sin Finish")
	}
}
func Excute(par *SQL_query,plan Plan) {
	fmt.Println("PLAN!!!")
	fmt.Println(plan)
	max:=Site_Size(par)
	Site := make([]Site_Send, max)
	for _,site_e:=range Site{
		site_e.len=0
	}
	//str:=",(.)*?\\)"
	//exp_p := regexp.MustCompile(str)

	//max:=Site_Size(par)
	turn:=-1
	if(par.jo.size==0){
		for _,tab:=range par.tab.tab {
			fmt.Println("-----------------step5: table transfer-----------------")
			//默认发给站点0
			fmt.Println("site_to: 0")
			for _, seg := range tab.segment {//单表无join
				//pos,_:=strconv.Atoi(seg[len(seg)-1:len(seg)])

				//if T为空（H）
				//if 无目标属性(V)
				send_from := "site_from" + seg[len(seg)-1:len(seg)]
				seg2 := strings.Replace(seg, ".", "_", -1)
				send := "select * from " + seg2 + "t"
				fmt.Println(send_from + " : " + send)
				targetSite, err5 := storage.Storage.GetRPCClient("st" + seg[len(seg)-1:len(seg)])
				if err5 != nil {
					log.Errorf("error_5 is %v", err5)
					fmt.Println("targetSite5失败")
				}
				var q peerCommunication.SelectQuery
				q.Query = send
				var qp string=""
				for i:=0;i<plan.site_max;i++{
					fmt.Println(i)
					if(plan.plans[i].step_size>0){
						qp=qp+"<"+strconv.Itoa(i)+">"
						m:=0
						for m=0;m<len(plan.plans[i].step_plan)-1;m++{
							qp=qp+plan.plans[i].step_plan[m]+"&"
						}
						qp=qp+plan.plans[i].step_plan[m]+"|"
					}else{
						qp=qp+"<"+strconv.Itoa(i)+">nil|"
					}
				}
				q.Query =q.Query+"!!"+qp
				fmt.Println("TEST!!!!!!!!!!!11")
				fmt.Println(q.Query)
				q.ToSite = ",st0"
				state, error := targetSite.Client.Select(context.Background(), &q)
				if (error != nil) {
					log.Errorf("error_5_s is %v", error)
					fmt.Println("error_5_s")
				}
				if (state.Ack == "Select Start") {
					fmt.Println("Select Start")
				}
			}
		}
	}
	for _,jo:=range par.jo.jo{//多个join
		//fmt.Println(jo.Strategy)
		turn++
		for _,strategy_each:=range jo.Strategy.join_seg{//每一个join
			//fmt.Println(i)
			//fmt.Println(strategy_each)
			var tabs1 []string = make([]string, 0)
			var tabs2 []string = make([]string, 0)
			tab1:=strings.Split(strategy_each,"(")
			for k:=0;k<2;k++{
				tab_e:=strings.Split(tab1[k],",")
				tabs1=append(tabs1,tab_e[0])
			}
			//fmt.Print(tabs1[1])
			//fmt.Println("!!!!!!!!!!!!!!!")
			tab2:=strings.Split(strategy_each,")")
			for k:=0;k<len(tab2)-1;k++{
				tab_e:=strings.Split(tab2[k],",")
				tabs2=append(tabs2,tab_e[len(tab_e)-1])
			}
			//fmt.Print(tabs2)
			//fmt.Println("!!!!!!!!!!!!!!!")
			exp_p := regexp.MustCompile( ",(.)*?,")
			all := exp_p.FindAllString(strategy_each, -1)
			jo:=strings.Replace(all[0],",","",-1)
			//fmt.Println(jo)

			for _,tabe:=range tabs2{
				site,_:=strconv.Atoi(tabe[len(tabe)-2:len(tabe)-1])
				var tab Tab_Send
				flag:=0

				for j:=0;j<Site[site].len;j++{
					if(Site[site].tab[j].name==tabe){
						Site[site].tab[j].to=append(Site[site].tab[j].to, tabs1[1])
						Site[site].tab[j].jo=append(Site[site].tab[j].jo, jo)
						flag=1
					}
				}
				if(flag==0){
					tab.jo=append(tab.jo,jo)
					tab.name=tabe
					tab.turn=turn
					tab.to=append(tab.to, tabs1[1])
					Site[site].tab=append(Site[site].tab, tab)
					Site[site].len++;
				}
				//for j:=0;j<Site[site-1].len;j++{
				//	if(Site[site-1].tab[j].name==tabe){
				//		Site[site-1].tab[j].to=append(Site[site-1].tab[j].to, tabs1[1])
				//		Site[site-1].tab[j].jo=append(Site[site-1].tab[j].jo, jo)
				//		flag=1
				//	}
				//}
				//if(flag==0){
				//	tab.jo=append(tab.jo,jo)
				//	tab.name=tabe
				//	tab.turn=turn
				//	tab.to=append(tab.to, tabs1[1])
				//	Site[site-1].tab=append(Site[site-1].tab, tab)
				//	Site[site-1].len++;
				//}

			}
		}
	}
	//fmt.Println(Site)
	fmt.Println("-----------------step5: table transfer-----------------")
	var Site_go []int = make([]int, 0)
	for k:=0;k<max;k++ {
		Site_go=append(Site_go, 0)
	}
	for j:=0;j<par.jo.size;j++ { //turn
		fmt.Print("turn")
		fmt.Println(j)
		for _, site_e := range Site {
			for _, tab := range site_e.tab {
				if (tab.turn == j) {
					//send
					name := strings.Replace(tab.name, ".", "_", -1)
					sql := "select * from " + name
					site_from, _ := strconv.Atoi(tab.name[len(tab.name)-2 : len(tab.name)-1])
					var site_tos []int = make([]int, 0)
					for _, to := range tab.to {
						if (tab.turn == 0) {
							site_to, _ := strconv.Atoi(to[len(to)-2 : len(to)-1])
							site_tos = append(site_tos, site_to)
						} else {
							site_to, _ := strconv.Atoi(to[len(to)-1 : len(to)])
							site_tos = append(site_tos, site_to)
						}

					}
					Send( site_from, site_tos, sql,plan)
					Site_go[site_from]++
				}
			}
		}
	}
	for k,sg:=range Site_go{
		if(sg==0){
			var site_tos []int = make([]int, 0)
			site_tos=append(site_tos, k)
			Send( k, site_tos, "",plan)
		}
	}
}



func Excute_single(par *SQL_query){
	max:=Site_Size(par)
	fmt.Println("max"+strconv.Itoa(max))
	var SQL [][]string
	var Tab [][]string
	for i=0;i<max;i++{
		tmp_SQL := make([]string, 0)
		SQL = append(SQL, tmp_SQL)
		tmp_Tab := make([]string, 0)
		Tab = append(Tab, tmp_Tab)
	}
	for i,table:=range par.tab.tab{
		for _,seg:=range table.segment{
			s:=seg[len(seg)-1 : len(seg)]
			pos,error :=strconv.Atoi(s)
			if error != nil{
				fmt.Println("字符串转换成整数失败")
			}
			var sql string
			if(par.pro.pro[i]=="*"){
				tab:=strings.Replace(seg[:len(seg)-2],".","_",-1)
				sql="select "+par.pro.pro[i]+" from "+tab
				fmt.Println(sql)
			}else{
				flag2:=0
				var Pro_final string=""
				pros:=strings.Split(par.pro.pro[i],",")
				key,_:=gddGet.GetTableKey(seg[:len(seg)-4])
				for _,pro:=range pros{
					if(strings.Contains(pro,".")){
						pro=strings.Split(pro,".")[1]
					}
					flag,_:=gddGet.GetFragmentColumnExist(seg[:len(seg)-2],pro)
					if(flag==1){
						Pro_final=Pro_final+pro+","
						if(pro==key){
							flag2=1
						}
					}
				}
				if(par.jo.size>0&&table.HV==1){
					if(flag2==0){
						//无key
						Pro_final=Pro_final+key+","
					}
				}
				//Pro_final=Pro_final[:len(Pro_final)-1]
				tab:=strings.Replace(seg[:len(seg)-2],".","_",-1)
				sql="select "+Pro_final
				for _,jo:=range par.jo.jo{
					joe:=jo.join_attr
					if(strings.Contains(joe,".")){
						joe =strings.Split(joe,".")[1]
					}
					tab2:=strings.Replace(tab,"_",".",-1)
					ifexist,_:=gddGet.GetFragmentColumnExist(tab2,joe)
					if(ifexist==1){
						//fmt.Println("222"+tab+" "+joe)
						if(!strings.Contains(sql,joe)){
							sql=sql+joe+","
						}
					}
				}
				sql=sql[:len(sql)-1]
				sql=sql+" from "+tab
				fmt.Println(sql)
			}
			k:=0
			for k=0;k<len(par.sel.selT[i].sel);k++{
				if(par.sel.selT[i].size>0){
					attr:=strings.Split(par.sel.selT[i].sel[k].attribute,".")[1]
					res,err1:=gddGet.GetFragmentColumnExist(seg[:len(seg)-2],attr)
					if(err1!=nil){
						log.Errorf("GetFragmentColumnExist1失败.2 is %v", err1)
						fmt.Println("GetFragmentColumnExist1失败")
					}
					if(res==1){
						if(k==0){
							sql=sql+" where "+attr+par.sel.selT[i].sel[k].operator+par.sel.selT[i].sel[k].value+" and "
						}else{
							sql=sql+attr+par.sel.selT[i].sel[k].operator+par.sel.selT[i].sel[k].value+" and "
						}
						fmt.Println(sql)
					}
				}
			}
			if(sql[len(sql)-4:len(sql)]=="and "){
				sql=sql[:len(sql)-5]
			}
			fmt.Println("Final SQL : "+sql)
			SQL[pos]=append(SQL[pos], sql)
			Tab[pos]=append(Tab[pos], seg+"t")
		}
		//fmt.Println(SQL)
		//fmt.Println(Tab)
	}
	for k:=0;k<max;k++{
		Send_single(k,SQL[k],Tab[k])
	}
}
