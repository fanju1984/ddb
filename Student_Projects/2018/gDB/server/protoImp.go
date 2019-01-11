package server

import (
	"context"
	"fmt"
	pb "gDB/pb"
	"strconv"
	"strings"
	"gDB/storage"
	"time"
	log "github.com/sirupsen/logrus"
)
var Insert_Finish int=0
var Insert_Finish_TEMP int=0

type Plan_S struct{
	step_size int
	step_plan []string
}

type Plan struct{
	site_max int
	plans []Plan_S
}

//TODO: This file should be edited to run all the peer communication logic.
func (s Server) Execute(ctx context.Context, in *pb.Query) (*pb.State, error) {
	if strings.Index(in.Query, "allocate_stmt") == 0 {
		createSQL := strings.SplitN(in.Query, ",", 2)[1]
		//fmt.Println(createSQL)
		db, err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			//fmt.Println("数据库连接失败")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		_, err = db.Exec(createSQL)
		if err != nil {
			//fmt.Println("目标站点的Create语句执行失败.")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
	}
	if strings.Index(in.Query, "insert_stmt") == 0 {
		insertSQL := strings.SplitN(in.Query, ",", 2)[1]
		//fmt.Println(createSQL)
		db, err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			fmt.Println("数据库连接失败")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		result, err := db.Exec(insertSQL)
		if err != nil {
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		rowNum, _ := result.RowsAffected()
		//fmt.Println("成功插入一条数据.")
		db.Close()
		return &pb.State{EffectedLine: rowNum, Error: ""}, nil
	}
	if strings.Index(in.Query, "delete_stmt") == 0 {
		deleteSQL := strings.SplitN(in.Query, ",", 2)[1]
		//fmt.Println(createSQL)
		db, err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			//fmt.Println("数据库连接失败")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		result, err := db.Exec(deleteSQL)
		if err != nil {
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		rowNum, _ := result.RowsAffected()
		//fmt.Println("成功插入一条数据.")
		return &pb.State{EffectedLine: rowNum, Error: ""}, nil
	}
	if strings.Index(in.Query, "dselect_stmt") == 0 {
		selectSQL := strings.SplitN(in.Query, ",", 2)[1]
		//fmt.Println(createSQL)
		db, err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			//fmt.Println("数据库连接失败")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		var id int64 = 0
		r, err := db.Query(selectSQL)
		r.Next()
		r.Scan(&id)
		return &pb.State{EffectedLine: id, Error: ""}, nil
	}
	if strings.Index(in.Query, "load_stmt") == 0 {
		insertStrs := strings.Split(in.Query, "|") // insert form 1- len(strs)-2

		db, err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			fmt.Println("数据库连接失败")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		var totalRow int64 = 0
		for index, insertSQL := range insertStrs {
			if index == 0 {
				continue
			}
			if index == len(insertStrs)-1 {
				break
			}
			_ , err := db.Exec(insertSQL)
			//fmt.Println(insertStr)
			if err != nil {
				fmt.Println("insert 语句执行失败.")
				return &pb.State{EffectedLine: -1, Error: ""}, err
			}
			totalRow ++
		}
		return &pb.State{EffectedLine: totalRow , Error: ""}, nil
	}
	if strings.Index(in.Query, "[VIEW]") == 0 {
		fmt.Println("in.Query : "+in.Query)
		createViewSQL := strings.Replace(in.Query,"[VIEW]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("view DB connect error is %v", err)
			//fmt.Println("数据库连接失败")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		fmt.Println("VIEW SQL : "+createViewSQL)
		_ , err =db.Exec(createViewSQL)
		if err != nil {
			log.Errorf("目标站点的Create_View语句执行失败 error is %v", err)
			//fmt.Println("目标站点的Create_View语句执行失败.")
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		return &pb.State{EffectedLine: 1, Error: ""}, nil
	}
	if strings.Index(in.Query, "[Temp_Create]") == 0 {
		createTempTable := strings.Replace(in.Query,"[Temp_Create]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			//fmt.Println("数据库连接失败")
			log.Errorf("Temp_Create DB connect error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		fmt.Println(createTempTable)
		_ , err =db.Exec(createTempTable)
		if err != nil {
			log.Errorf("目标站点的Temp_Create语句执行失败 error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		return &pb.State{EffectedLine: 2, Error: ""}, nil
	}
	if strings.Index(in.Query, "[INSERT]") == 0 {
		insertTable := strings.Replace(in.Query,"[INSERT]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("INSERT DB connect error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		_ , err =db.Exec(insertTable)
		if err != nil {
			log.Errorf("目标站点的insert语句执行失败 error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		return &pb.State{EffectedLine: 2, Error: ""}, nil
	}
	if strings.Index(in.Query, "[INDEX]") == 0 {
		insertTable := strings.Replace(in.Query,"[INDEX]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("INDEX DB connect error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		_ , err =db.Exec(insertTable)
		if err != nil {
			log.Errorf("目标站点的index语句执行失败 error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		return &pb.State{EffectedLine: 3, Error: ""}, nil
	}
	if strings.Index(in.Query, "[MIN]") == 0 {
		MinF := strings.Replace(in.Query,"[MIN]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("MIN DB connect error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		rows, err := db.Query(MinF)
		if err != nil {
			log.Errorf("目标站点的MIN语句执行失败 error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		var min int64
		for rows.Next(){
			err = rows.Scan(&min)
		}
		//rows.Close()
		return &pb.State{EffectedLine: min, Error: ""}, nil
	}
	if strings.Index(in.Query, "[MAX]") == 0 {
		MaxF := strings.Replace(in.Query,"[MAX]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("MAX DB connect error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		rows, err := db.Query(MaxF)
		if err != nil {
			log.Errorf("目标站点的MAX语句执行失败 error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		var max int64
		for rows.Next(){
			err = rows.Scan(&max)
		}
		//rows.Close()
		return &pb.State{EffectedLine: max, Error: ""}, nil
	}
	if strings.Index(in.Query, "[SIN]") == 0 {
		sin := strings.Replace(in.Query,"[SIN]","",-1)
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("SIN DB connect error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		rows, err := db.Query(sin)
		time.Sleep(5)
		if err != nil {
			log.Errorf("目标站点的MAX语句执行失败 error is %v", err)
			return &pb.State{EffectedLine: -1, Error: ""}, err
		}
		count:=0
		cols, err := rows.Columns()
		if(err!=nil){
			log.Errorf("read columns执行失败 error is %v", err)
		}
		fmt.Println(cols)
		vals := make([][]byte, len(cols))
		scans := make([]interface{}, len(cols))

		for i := range vals{
			scans[i] = &vals[i]
		}

		var results []map[string]string

		for rows.Next(){
			count++
			err = rows.Scan(scans...)
			if err != nil{
				log.Errorf("111 err %v",err)
			}

			row := make(map[string]string)
			for k, v := range vals{
				key := cols[k]
				row[key] = string(v)
			}
			results = append(results, row)
		}
		//rows.Close()
		for k:=0;k<5;k++{
			fmt.Println(results[k])
		}
		fmt.Println("COUNT : "+strconv.Itoa(count))
		return &pb.State{EffectedLine: 10, Error: ""}, nil
	}
	return &pb.State{EffectedLine: -1 , Error: ""}, nil
}



func (s Server) Select(ctx context.Context, in *pb.SelectQuery) (*pb.ACK, error) {
	go func(){
		db,err := storage.Storage.DB.Connect("postgres")
		if err != nil {
			log.Errorf("SQL DB connect error is %v", err)
		}
		fmt.Println("QUERY!!!!!!")
		fmt.Println(s.cfg.Name)
		fmt.Println(in.Query)
		Query:=strings.Split(in.Query,"!!")[0]
		plan_str:=strings.Split(in.Query,"!!")[1]
		plan_strs:=strings.Split(plan_str,"|")
		var plan_F Plan
		plan_F.plans=make([]Plan_S,0)
		num:=0
		for x:=0;x<len(plan_strs)-1;x++{
			if(strings.Contains(plan_strs[x],">")){
				site1:=strings.Split(plan_strs[x],">")[0]
				site2 :=strings.Split(site1,"<")[1]
				site_now,_:=strconv.Atoi(site2)
				if(site_now>num){
					num=site_now
				}
				ss:=strings.Split(plan_strs[x],">")[1]
				if(strings.Contains(ss,"&")){
					sss:=strings.Split(ss,"&")
					//site,_:=strconv.Atoi(site2)
					fmt.Println(sss)
					//Exist处理
					var si int
					for si=0;si<len(sss);si++{
						if(!strings.Contains(sss[si],"[EXIST]")){
							break
						}
					}
					sss_E:=make([]string,1)
					sss_E[0]="[EXIST]select exists (select 1 from  "
					for k:=0;k<si;k++{
						h:=strings.Split(sss[k],"from ")[1]
						h=strings.Split(h,")")[0]
						tab:=strings.Split(h,",")
						for _,ts:=range tab{
							if(!strings.Contains(sss_E[0],ts)){
								if(!strings.Contains(ts,"TEMP")){
									sss_E[0]=sss_E[0]+ts+","
								}
							}
						}
					}
					sss_E[0]=sss_E[0][:len(sss_E[0])-1]+")"
					for k:=si;k<len(sss);k++{
						sss_E=append(sss_E, sss[k])
					}
					//合并0~si-1的
					var temp Plan_S
					temp.step_size=len(sss_E)
					temp.step_plan=sss_E
					plan_F.plans=append(plan_F.plans, temp)
					//plan_F.site_max=len(temp)
					//plan_F.plans[site].step_plan=sss
				}else{
					if(!strings.Contains(ss,"nil")){
						var temp Plan_S
						var res []string = make([]string, 0)
						res=append(res, ss)
						temp.step_plan=res
						temp.step_size=1
						plan_F.plans=append(plan_F.plans, temp)
					}else{
						var temp Plan_S
						var res []string = make([]string, 0)
						res=append(res, "")
						temp.step_plan=res
						temp.step_size=0
						plan_F.plans=append(plan_F.plans, temp)
					}
				}

			}
		}
		plan_F.site_max=num+1

		if(Query!=""){
			rows, err := db.Query(Query)
			if err != nil {
				log.Errorf("目标站点的SQL Query语句执行失败 error is %v", err)
			}
			Send_Columns:="<"
			columns,_:=rows.Columns()
			for _,column:=range columns{
				Send_Columns=Send_Columns+column+","
			}
			table_name:="["+strings.Split(Query,"from ")[1]+"]"//table name
			Send_Columns=Send_Columns[:len(Send_Columns)-1]+">"
			//向站点传数据
			sites:=strings.Split(in.ToSite,",")
			/*
			发送的数据!!!
			 */
			var q pb.Result
			q.Result = table_name + Send_Columns
			cols, err := rows.Columns()
			if err != nil {
				log.Errorf("222 err %v",err)
			}
			fmt.Println(cols)
			vals := make([][]byte, len(cols))
			scans := make([]interface{}, len(cols))

			for i := range vals {
				scans[i] = &vals[i]
			}
			row_num:=0
			//var results []map[string]string
			for rows.Next() {
				row_num++
				if(row_num%10000==0){
					fmt.Println("row_num: "+strconv.Itoa(row_num))
				}
				err = rows.Scan(scans...)
				if err != nil {
					log.Errorf("333 err %v",err)
				}

			//	row := make(map[string]string)
			//	for k, v := range vals {
			//		key := cols[k]
			//		row[key] = string(v)
			//	}
			//	//results = append(results, row)
			//}

			row := make(map[string]string)
			for k, v := range vals {
				key := cols[k]
				row[key] = string(v)
				q.Result = q.Result +row[key]+","
			}
			//results = append(results, row)
			//for _, colmn := range cols {
			//	//fmt.Println(v[colmn])
			//	q.Result = q.Result + results[j][colmn] + ","
			//}
			q.Result = q.Result + "|"
		}
		q.Result = q.Result[:len(q.Result)-2]
			//j:=0
			//for j=0;j<len(results)-1;j++{
			//	for _, colmn := range cols {
			//		//fmt.Println(v[colmn])
			//		q.Result = q.Result + results[j][colmn] + ","
			//	}
			//	q.Result = q.Result + "|"
			//}
			//t:=0
			//for t=0;t<len(cols)-1;t++{
			//	q.Result = q.Result + results[j][cols[t]] + ","
			//}
			//q.Result = q.Result + results[j][cols[t]]
			for _,site:=range sites {
				if (strings.Contains(site, "st")) {
					fmt.Println("SentResult to " + site)
					if (s.cfg.Name != site) {
						targetSite, err0 := storage.Storage.GetRPCClient(site)
						if err0 != nil {
							log.Errorf("targetSite0失败 error is %v", err)
						}
						fmt.Println("SendResult Start!!!")
						state, err := targetSite.Client.SendResult(context.Background(), &q)
						if (err != nil) {
							log.Errorf("error_SendResult error is %v", err)
							return
						}

						if (state.Ack == "Insert Finish") {
							fmt.Println("Insert Finish")
						}
					}
				}
			}
		}

		//rows.Close()
		step:=0
		//site_now:=s.cfg.Name
		//in:= peerCommunication.SelectQuery{}
		//var in *pb.FinishMessage
		//in.ThisTurn="["+site_now+"]"+"<"+strconv.Itoa(step)+">"
		fmt.Print("plan_F: ")
		fmt.Println(plan_F)
		fmt.Println("site_max:"+strconv.Itoa(plan_F.site_max))
		for {
			site,_:=strconv.Atoi(strings.Replace(s.cfg.Name,"st","",-1))
			//if(plan_F.site_max>=site){
			if(plan_F.plans[site].step_size-1>=step){
				//if((len(plan_F.plans[site].step_plan)-1)>=step){
				fmt.Println("当前site执行next plan: st"+strconv.Itoa(site))
				fmt.Println("当前site执行step_size: "+strconv.Itoa(len(plan_F.plans[site].step_plan)-1))
				fmt.Println("step: "+strconv.Itoa(step))
				//log.Debugf("Site越界："+strconv.Itoa(site))
				fmt.Println("plan"+" : "+"step"+strconv.Itoa(step)+" start!!")
				sql:=plan_F.plans[site].step_plan[step]
				fmt.Println("执行的plan: "+sql)
				step++
				times:=0
				//for{
				//	if(Insert_Finish==1){//insert 已经finish，表稳定
				if(strings.Contains(sql,"[EXIST]")){
					if(step==1){
						sql=strings.Replace(sql,"[EXIST]","",-1)
						Max_size:=0
						ta:=strings.Split(sql,"from")[1]
						ta=strings.Split(ta,")")[0]
						ts:=strings.Split(ta,",")
						for _,tab:=range ts{
							site:=tab[len(tab)-2]
							if(site!=s.cfg.Name[len(s.cfg.Name)-1]){
								Max_size++
							}
						}
						fmt.Println(Max_size)
						for{
							time.Sleep(5)
							if(Insert_Finish==1){
								fmt.Println("table exists2")
								if(times<Max_size){
									Insert_Finish=0
									times++
								}
								if(times==Max_size){
									fmt.Println("table exists3")
									//_,err:=db.Exec(sql)
									//if(err==nil){
									//	fmt.Println("table exists")
									//	break
									//}
									break
								}
							}
						}

					}else{
						if(step==plan_F.plans[site].step_size-1&&site==0){
							//sql=strings.Replace(sql,"[EXIST]","",-1)
							Max_size:=0
							ta:=strings.Split(sql,"from")[1]
							ta=strings.Split(ta,")")[0]
							ts:=strings.Split(ta,",")
							for _,tab:=range ts{
								site:=tab[len(tab)-1]
								if(site!=s.cfg.Name[len(s.cfg.Name)-1]){
									Max_size++
								}
							}
							fmt.Println(Max_size)
							for{
								time.Sleep(1)
								if(Insert_Finish_TEMP==1){
									fmt.Println("table_temp exists2")
									if(times<Max_size){
										Insert_Finish_TEMP=0
										times++
									}
									if(times==Max_size){
										fmt.Println("table_temp exists3")
										break
									}
								}
							}
						}else {
							fmt.Println("table exists4")
						}
					}

				}else{
					//_,err = db.Exec(sql)
					//Final才输出
					if(strings.Contains(sql,"[FINAL]")){
						sql=strings.Replace(sql,"[FINAL]","",-1)
						fmt.Println(s.cfg.Name)
						fmt.Println(sql)
						//sql=Espe(sql)
						rows2, err := db.Query(sql)
						//time.Sleep(1)
						if err != nil {
							log.Errorf("目标站点的FINAL语句执行失败 error is %v", err)
						}
						count:=0
						cols, err := rows2.Columns()
						if(err!=nil){
							log.Errorf("read columns执行失败 error is %v", err)
						}
						fmt.Println(cols)
						vals := make([][]byte, len(cols))
						scans := make([]interface{}, len(cols))

						for i := range vals{
							scans[i] = &vals[i]
						}

						var results []map[string]string

						for rows2.Next(){
							count++
							err = rows2.Scan(scans...)
							if err != nil{
								log.Errorf("444 err %v",err)
							}

							row := make(map[string]string)
							for k, v := range vals{
								key := cols[k]
								row[key] = string(v)
							}
							results = append(results, row)
						}
						rows2.Close()
						for k:=0;k<5;k++{
							fmt.Println(results[k])
						}
						fmt.Println("COUNT : "+strconv.Itoa(count))
					}else{
						if(strings.Contains(sql,"[sent to 0]")){
							sql=strings.Replace(sql,"[sent to 0]","",-1)
							Query_last_insert:=strings.Split(sql,"$")[2]
							if (s.cfg.Name != "st0") {
								rows, err := db.Query(Query_last_insert)
								if err != nil {
									log.Errorf("目标站点的SQL Query语句执行失败 error is %v", err)
								}
								table_name:="["+strings.Split(sql,"$")[1]+"]"
								Send_Columns:="<"
								columns,_:=rows.Columns()
								for _,column:=range columns{
									Send_Columns=Send_Columns+column+","
								}
								Send_Columns=Send_Columns[:len(Send_Columns)-1]+">"
								var q pb.Result
								q.Result = table_name + Send_Columns
								cols, err := rows.Columns()
								if err != nil {
									log.Errorf("555 err %v",err)
								}
								fmt.Println(cols)
								vals := make([][]byte, len(cols))
								scans := make([]interface{}, len(cols))

								for i := range vals {
									scans[i] = &vals[i]
								}
								row_num:=0
								//var results []map[string]string
								for rows.Next() {
									row_num++
									if(row_num%10000==0){
										fmt.Println("row_num: "+strconv.Itoa(row_num))
									}
									err = rows.Scan(scans...)
									if err != nil {
										log.Errorf("666 err %v",err)
									}

									row := make(map[string]string)
									for k, v := range vals {
										key := cols[k]
										row[key] = string(v)
										q.Result = q.Result +row[key]+","
									}
									//results = append(results, row)
									//for _, colmn := range cols {
									//	//fmt.Println(v[colmn])
									//	q.Result = q.Result + results[j][colmn] + ","
									//}
									q.Result = q.Result + "|"
								}
								q.Result = q.Result[:len(q.Result)-2]
								//j:=0
								//for j=0;j<len(results)-1;j++{
								//	if(j%2500==0){
								//		fmt.Println("insert "+strconv.Itoa(j)+" tuples")
								//	}
								//	for _, colmn := range cols {
								//		//fmt.Println(v[colmn])
								//		q.Result = q.Result + results[j][colmn] + ","
								//	}
								//	q.Result = q.Result + "|"
								//}
								//t:=0
								//for t=0;t<len(cols)-1;t++{
								//	q.Result = q.Result + results[j][cols[t]] + ","
								//}
								//q.Result = q.Result + results[j][cols[t]]
								targetSite, err0 := storage.Storage.GetRPCClient("st0")
								if err0 != nil {
									log.Errorf("targetSite1失败 error is %v", err)
								}
								fmt.Println("SendResult_last Start!!!")
								state, err := targetSite.Client.SendResult(context.Background(), &q)
								if (err != nil) {
									log.Errorf("error_SendResult_last error is %v", err)
									return
								}

								if (state.Ack == "Insert Finish") {
									fmt.Println("Insert Finish_last")
									//rows.Close()
									break
								}
							}
						}else{
							//time.Sleep(1)
							fmt.Println("not final!!")
							fmt.Println(sql)
							if(!strings.Contains(sql,"cross join")){
								fmt.Println("没有cross join")
								if(strings.Contains(sql,";")){
									sqls:=strings.Split(sql,";")
									for i,s:=range sqls{
										fmt.Println(strconv.Itoa(i)+" "+s)
										row, err := db.Query(s)
										if err != nil {
											log.Errorf("not final目标站点的语句执行失败 error is %v", err)
										}
										row.Close()
									}
								}
							}else{//笛卡儿积只取前面的表即可
								fmt.Println("有cross join")
								if(strings.Contains(sql,";")){
									sqls:=strings.Split(sql,";")
									for i,s:=range sqls{
										if(strings.Contains(s,"cross join")){
											s1:=strings.Split(s,"from")[0]
											s2:=strings.Split(s," cross join ")[1]
											s3:=s1+"from "+s2
											fmt.Println(strconv.Itoa(i)+" "+s3)
											row, err := db.Query(s3)
											if err != nil {
												log.Errorf("not final目标站点的语句执行失败 error is %v", err)
											}
											row.Close()
										}else{
											fmt.Println(strconv.Itoa(i)+" "+s)
											row, err := db.Query(s)
											if err != nil {
												log.Errorf("not final目标站点的语句执行失败 error is %v", err)
											}
											row.Close()
										}

									}
								}
							}
							fmt.Println("not final finish!!")
							//sql="CREATE VIEW t0 AS select * from teacher_2_1t inner join course_2_1t using(teacher_id)"

						}

					}
				}
			}else{
				fmt.Println("-------------------------------END----------------------------------------")
				break
			}
		}

	}()
	return &pb.ACK{Ack: "Select Start"}, nil
}

func (s Server) SendResult(ctx context.Context, in *pb.Result) (*pb.ACK, error) {
	fmt.Println("ReceiveResult Start!!!")
	Insert_Finish=0
	Insert_Finish_TEMP=0
	/*
	in.Result包含三部分：
	（1）[tablename]
	（2）<attribute name>
	（3）string,string,...,string,|string,string,...,string,
	 */
	//Create Table
	//create_sql:="[Temp_Create]"
	create_sql:=""
	//fmt.Println(in.Result)
	table_name:=strings.Split(in.Result,"]")[0]
	table_name=strings.Split(table_name,"[")[1]
	create_sql=create_sql+"drop table if exists "+table_name+";create table "+table_name+"("
	attr:=strings.Split(in.Result,">")[0]
	attr=strings.Split(attr,"<")[1]
	attrs:=strings.Split(attr,",")
	for _,at:=range attrs{
		create_sql=create_sql+at+" "+Type_find(at)+","
	}
	db,err := storage.Storage.DB.Connect("postgres")
	if err != nil {
		log.Errorf("Temp_Create2 connect error is %v", err)
	}
	create_sql=create_sql[:len(create_sql)-1]
	create_sql=create_sql+")"
	fmt.Println(create_sql)
	db.Exec(create_sql)//无需使用RPC！！！！
	//一句一句insert
	insert_sqls:=strings.Split(in.Result,">")[1]
	insertMulti:=strings.Split(insert_sqls,",|")
	fmt.Print("insert ： ")
	fmt.Println(len(insertMulti))
	insert_sql:="INSERT INTO "+table_name+" VALUES ("
	oneRow := strings.Split(insertMulti[0], ",")
	for i, _ := range oneRow {
		insert_sql=insert_sql+"($"+strconv.Itoa(i+1)+")"+","
	}
	insert_sql=insert_sql[:len(insert_sql)-1]+")"
	fmt.Println(insert_sql)
	stmt, err := db.Prepare(insert_sql)
	if err != nil {
		_ = db.Close()
		log.Print(insert_sql)
		log.Errorf("Insert2 error is %v", err)
	}
	for _,insert_Row:=range insertMulti{
		inserts:=strings.Split(insert_Row,",")
		is := make([]interface{}, len(inserts))
		for i, d := range inserts {
			if(d==""){
				log.Println("有缺失："+d)
				log.Println(inserts)
				d="-1"
				is[i] = d
			}else{
				is[i] = d
			}
		}
		if _, err := stmt.Exec(is...); err != nil {
			log.Debugf("insert_Row: "+insert_Row)
			s:=""
			for _,in:=range inserts{
				s=s+"|"+in
			}
			log.Debugf("insert: "+s)
			s2:=""
			for _,in:=range is{
				s2=s2+"|"+in.(string)
			}
			log.Debugf("is: "+s2)
			log.Errorf("insert connect error is %v", err)
			return &pb.ACK{Ack: "error"}, err
		}
	}
	fmt.Println("insert finish!!")
	Insert_Finish=1
	if(strings.Contains(table_name,"TEMP")){
		Insert_Finish_TEMP=1
	}
	return &pb.ACK{Ack: "Insert Finish"}, nil
}

func (s Server) RequestNext(ctx context.Context, in *pb.FinishMessage) (*pb.NextStep, error) {
	return &pb.NextStep{Plan: ""}, nil
}

func (s Server) AddRPCClient(ctx context.Context, in *pb.NewClient) (*pb.ACK, error) {
	storage.AddRPCClient(in.Name, in.Url)
	return &pb.ACK{Ack: ""}, nil
}

func (s Server) Load(ctx context.Context, in *pb.Data) (*pb.ACK, error) {
	return &pb.ACK{Ack: ""}, nil
}