package server

import "strings"

func Type_find(attr string) (string) {
	if(strings.Contains(attr,"id")||attr=="age"||attr=="title"||attr=="degree"||attr=="credit_hour"||attr=="mark"){
		return "int"
	}
	if(attr=="student_name"||attr=="teacher_name"){
		return "char(25)"
	}
	if(attr=="course_name"){
		return "char(80)"
	}
	if(attr=="sex"){
		return "char(1)"
	}
	if(attr=="location"){
		return "char(8)"
	}
	return "NULL"
}

//func Espe(sql string)(string){
//	if(sql=="select course_name,credit_hour,teacher_name from TEMP0_0,TEMP1_1 where TEMP0_0.course_id=TEMP1_1.course_id"){
//		sql="select course_name,credit_hour,teacher_name from Course_1_0t,TEMP1_1 where Course_1_0t.course_id=TEMP1_1.course_id"
//	}
//	return sql
//}