/*
传语句给pg对应接口，得到某个分片的某个属性的最大值和最小值
 */
package SQL

import (
	"context"
	"fmt"
	"gDB/pb"
	"gDB/server"
	"gDB/storage"
	"github.com/sirupsen/logrus"
	"strings"

	//"strconv"
)
func GetMin(svr *server.Server,fragment string, attr string,site string) int64{
	targetSite,err10:=storage.Storage.GetRPCClient("st"+site)
	if err10 != nil {
		fmt.Println("targetSite10失败")
	}
	var q peerCommunication.Query
	fragment=strings.Replace(fragment,".","_",-1)
	sql:="select MIN("+attr+") from "+fragment
	q.Query="[MIN]"+sql
	state,err:=targetSite.Client.Execute(context.Background(),&q)
	if(err!=nil){
		logrus.Errorf("Min_err is %v",err)
	}
	if(state.EffectedLine!=-1){
		fmt.Print("Select MIN Finish")
		return state.EffectedLine
	}
	return -1
}


func GetMax(svr *server.Server,fragment string, attr string,site string) int64{
	targetSite,err10:=storage.Storage.GetRPCClient("st"+site)
	if err10 != nil {
		fmt.Println("targetSite11失败")
	}
	var q peerCommunication.Query
	fragment=strings.Replace(fragment,".","_",-1)
	sql:="select MAX("+attr+") from "+fragment
	q.Query="[MAX]"+sql
	state,err:=targetSite.Client.Execute(context.Background(),&q)
	if(err!=nil){
		logrus.Errorf("Max_err is %v",err)
	}
	if(state.EffectedLine!=-1){
		fmt.Print("Select MAX Finish")
		return state.EffectedLine
	}
	return -1
}