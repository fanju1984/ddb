/*
SQL语句解析及执行计划生成与执行的入口
 */
package SQL

import (
	"fmt"
	//"strconv"
)


func Test(Sql string) {
	par := Parser(Sql)
	if par.tab.size != 1 {
		par = updateOrderofTJ(par)
	}
	par = updateS(par)//select排序
	par = updateP(par)

	nodes2 := Translate_C(par)
	var sqlTree Tree
	var root *Tree
	root = CreateTree(&sqlTree, nodes2, 0)
	trans_visual(root) //root为SQL树

	fmt.Println("-----------------step2: Fragments after H/V pruning-----------------")
	for i := 0; i < par.tab.size; i++ {
		if par.tab.tab[i].HV == 0 { //水平
			fmt.Print("水平： ")
			//水平select剪枝
			HFPrune_Sel(i, par)
			fmt.Println(par.tab.tab[i].segment)
		} else if par.tab.tab[i].HV == 1 {
			fmt.Print("垂直： ")
			//垂直pro剪枝
			if(par.pro.pro[0]=="*"&&par.pro.size==1){
			}else{
				VFPrune_Sel(i, par)
			}
			fmt.Println(par.tab.tab[i].segment)
		}
	}
	//fmt.Println("-----------------step+: Create index-----------------")
	//Create_index(par)
	fmt.Println("-----------------step3: Join strategy-----------------")
	par=F_Join(par)
	fmt.Println("-----------------step4: operation before join for each table_F (sel,pro)-----------------")
	Excute_single(par)
	if(par.tab.size==1&&par.tab.tab[0].size==1){
		fmt.Println("仅单分片")
		Excute_sin(par)
	}else{
		var plan Plan
		plan=Plan_Form(par)
		Excute(par,plan)
	}

}
