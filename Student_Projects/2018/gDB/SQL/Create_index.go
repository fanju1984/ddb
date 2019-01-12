/*
对指定连接表用join的属性建立索引
 */
package SQL

import (
	"fmt"
	"gDB/gddGet"
	"gDB/pb"
	"gDB/server"
	"context"
	"strconv"
	"strings"
	"gDB/storage"
)

func Create_index(svr *server.Server,par *SQL_query){
	jos:=par.jo.jo
	for i,jo:=range jos{
		fmt.Println(jo.join_attr)
		fmt.Println(par.tab.tab[i].segment)
		for m,seg1:=range par.tab.tab[i].segment{
			exist,_:=gddGet.GetFragmentColumnExist(seg1[:len(seg1)-2],jo.join_attr)
			if(exist==1){//存在建立索引
				targetSite,_:=storage.Storage.GetRPCClient("st"+seg1[len(seg1)-1:len(seg1)])
				var q peerCommunication.Query
				fmt.Print(" : ")
				tab_name:=strings.Replace(seg1[:len(seg1)-2],".","_",-1)
				index_name:=tab_name+"_index_"+strconv.Itoa(m)
				fmt.Println("drop index if exists "+index_name+";CREATE INDEX "+index_name+" ON "+tab_name+" ("+jo.join_attr+" )")
				q.Query="[INDEX]"+"drop index if exists "+index_name+";CREATE INDEX "+index_name+" ON "+tab_name+" ("+jo.join_attr+" )"
				state,error:=targetSite.Client.Execute(context.Background(),&q)
				if(error!=nil){
					fmt.Println("error_INDEX1_Create")
				}
				if(state.EffectedLine==3){
					fmt.Print("Create_INDEX1 success")
				}
			}
		}


		fmt.Println(par.tab.tab[i+1].segment)
		for m,seg1:=range par.tab.tab[i+1].segment{
			exist,_:=gddGet.GetFragmentColumnExist(seg1[:len(seg1)-2],jo.join_attr)
			if(exist==1){//存在建立索引
				targetSite,_:=storage.Storage.GetRPCClient("st"+seg1[len(seg1)-1:len(seg1)])
				var q peerCommunication.Query
				fmt.Print(" : ")
				tab_name:=strings.Replace(seg1[:len(seg1)-2],".","_",-1)
				index_name:=tab_name+"_index_"+strconv.Itoa(m)
				fmt.Println("drop table if exists "+index_name+";CREATE INDEX "+index_name+" ON "+tab_name+" ("+jo.join_attr+" )")
				q.Query="[INDEX]"+"drop table if exists "+index_name+";CREATE INDEX "+index_name+" ON "+tab_name+" ("+jo.join_attr+" )"
				state,error:=targetSite.Client.Execute(context.Background(),&q)
				if(error!=nil){
					fmt.Print("error_INDEX1_Create")
				}
				if(state.EffectedLine==3){
					fmt.Println("Create_INDEX1 success")
				}
			}
		}
	}

}