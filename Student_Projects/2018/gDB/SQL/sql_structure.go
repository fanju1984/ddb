// SQL语句的结构
package SQL

type SQL_query struct {
	pro *Projection
	tab *Table
	jo  *Join
	sel *Select
}

func NewSQL(pros *Projection, tabs *Table, jos *Join, sels *Select) *SQL_query {
	return &SQL_query{
		pro: pros,
		tab: tabs,
		jo:  jos,
		sel: sels,
	}
}

//Tables
type Table struct {
	tab  []Distributed_Table
	size int
}

type Distributed_Table struct {
	tab     string
	segment []string
	size    int
	HV      int
}

func NewDTab(tab string, segment []string, HV int) Distributed_Table {
	return Distributed_Table{
		tab:     tab,
		segment: segment,
		size:    len(segment),
		HV:      HV, //0水平1垂直2混合
	}
}

func NewTab(tabs []Distributed_Table) *Table {
	return &Table{
		tab:  tabs,
		size: len(tabs),
	}
}

//Join
type Join struct {
	jo   []Join_each
	size int
}
type Join_Stra struct {
	join_seg  []string
	temp_table []string
	size      int
	UJ        int
	join_attr string
}
type Join_each struct {
	tab1      string
	tab2      string
	join_attr string
	Strategy  Join_Stra
}

func NewEJoin(tab1 string, tab2 string, join_attr string) Join_each {
	return Join_each{
		tab1:      tab1,
		tab2:      tab2,
		join_attr: join_attr,
	}
}

func NewJoin(jo []Join_each) *Join {
	return &Join{
		jo:   jo,
		size: len(jo),
	}
}

//Select
type Select struct {
	selT []Select_eachT
	size int
}

type Select_eachT struct {
	sel  []Select_Each
	size int
	pos  int
}

type Select_Each struct {
	attribute string
	operator  string
	value     string
}

func NewSelectEach(attribute string, operator string, value string) Select_Each {
	return Select_Each{
		attribute: attribute,
		operator:  operator,
		value:     value,
	}
}

func NewSelectEachT(sels []Select_Each, pos int) Select_eachT {
	return Select_eachT{
		sel:  sels,
		size: len(sels),
		pos:  pos,
	}
}

func NewSel(sels []Select_eachT) *Select {
	return &Select{
		selT: sels,
		size: len(sels),
	}
}

//Projections
type Projection struct {
	pro  []string
	size int
}

func NewPro(pros []string) *Projection {
	return &Projection{
		pro:  pros,
		size: len(pros),
	}
}
