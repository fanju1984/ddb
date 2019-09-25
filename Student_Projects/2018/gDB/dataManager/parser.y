%{
    /*
        yacc文件用于语法解析
        最终返回一个statement结构
    */
    package dataManager
    import (
        "strconv"
    )
    // 这个函数用来将最终获得的statement赋值给lex
    func setStmtResult(l yyLexer, v Statement) {
      l.(*lex).statement = v
    }

    // 这个函数用来设置FragmentName, FragmentNum，我们在语法分析的最后阶段使用
    func setFragment(f *Fragment) {
	    for i, frag :=range f.FragmentSpec.Fragments{
		    frag.FragmentName = f.TableName + "." + strconv.Itoa(i+1)
	    }
	    f.FragmentNum = len(f.FragmentSpec.Fragments)
    }

%}

%union{
    stmtType        int
    stmt            Statement
    str             string
    strs            []string    //
    num             string         // 用来存储NUMBER的数值
    dbddl           DBDDL
    column          *ColumnDefinition // 单个column的定义 如 id int
    columns         TableSpec   // 存储多个column的定义
    ddl             DDL         // ddl statement
    b               bool        // 用于 notnull 和 primary key
    insert          Insert      // insert statement
    delete          Delete      // delete statement
    fragment        Fragment
    fragment_def    FragmentDefinition
    fragment_def_p  *FragmentDefinition
    fragment_spec   FragmentSpec
    allocate        Allocate
    load            Load
}


%token LEX_ERROR
%token UNUSED // 出现在我们的关键字列表中, 但是目前我们还没有在yacc中分析它
// operators token
%left <str> OR
%left <str> AND

// DDL AND DML Tokens
%token CREATE ALTER DROP UPDATE DELETE INSERT LOAD INTO
%token DATABASE SCHEMA TABLE PRIMARY VALUES
%token PARTITION BY IN FROM WHERE TO
%token NOT NULL KEY

// Type Tokens
%token <str> INT FLOAT DOUBLE
%token <str> CHAR VARCHAR NCHAR TEXT
%token <num> NUMBER
%token <str> IDENT   // 标识符, 不带单引号
%token <str> STRING  // 字符串, 带单引号
%token <str> NAMEWITHDOT // 中间带点的 如student.1
%token FRAGMENT
%token HORIZONTALLY
%token VERTICALLY

// 最终得到的结果
%type <stmt> statement
%token ALLOCATE

// dbddl 的中间结果
%type <dbddl> create_db
%type <str> db_name

// ddl 的中间结果的类型定义
%type <ddl> create_table
%type <str> table_name
%type <str> column_name
%type <str> column_type
%type <b> null_opt
%type <b> column_key_opt
%type <column> column_def  // 单个属性的定义
%type <columns> column_def_list // 多个属性,我们在union中将其定义为表空间
%type <str> char_type
%type <str> length_opt

// fragment 的中间结果类型
%type <str> fragment_l_op      // Logic operator 这里我们目前只考虑了 and & 和 or
%type <str> a_op      // Arithmetic operator 这里我们只考虑了> >= < <= = <> 6种
%type <fragment> fragment_table
%type <fragment> h_fragment_table
%type <fragment> v_fragment_table
%type <fragment_spec> h_fragment_list
%type <fragment_spec> v_fragment_list
%type <fragment_def> h_fragment
%type <fragment_def_p> v_fragment
%type <strs> h_fragment_limit
%type <strs> v_frag_columns

// allocate的中间结果定义
%type <allocate> allocate_frag

// insert 的中间结果的类型定义
%type <insert> insert_table
%type <strs> insert_values
%type <str> insert_value


// delete 的中间结果类型定义
%type <delete> delete_table
%type <strs> delete_conditions // strs[0] 存储第一个属性, strs[1]存储整个条件
%type <strs> delete_condition
%type <str> d_l_op

// load 的中间结果定义
%token LOAD
%type <load> load_stmt

%start main

%%

main: statement
    {
        setStmtResult(yylex, $1)
    }
    | LEX_ERROR{}
statement:
    create_db ';'
    {
        $$ = $1
    }

  | create_table ';'
    {
        $$ = $1
    }
  | insert_table ';'
    {
        $$ = $1
    }
  | delete_table ';'
    {
        $$ = $1
    }
  | fragment_table ';'
    {
        $$ = $1
    }
  | allocate_frag ';'
    {
        $$ = $1
    }
  | load_stmt ';'
    {
        $$ = $1
    }

create_db: CREATE DATABASE db_name
           {
                $$ = DBDDL{Action: "CREATE", DbName: $3, Part_DB: nil}
           }

db_name: IDENT
        {
               $$ = $1
        }

create_table:
    CREATE TABLE table_name '(' column_def_list ')'
   {
        $$ = DDL{Action: "CREATE", TableName: $3, TableSpec: &($5)}
   }
table_name: IDENT
      {
         $$ = $1
      }

column_def_list: column_def
               {
                    $$.Columns = append($$.Columns, $1)
               }
               | column_def_list ',' column_def
               {
                    $$.Columns = append($$.Columns, $3)
               }

column_def:
  column_name column_type null_opt column_key_opt
  {
        $$ = &ColumnDefinition{Name: $1, DataType: $2, Not_Null: $3, Primarykey: $4}
  }

column_name: IDENT
    {
        $$ = $1
    }

column_type: INT
  {
    $$ = "int"
  }
  | FLOAT
  {
    $$ = "float"
  }
  | DOUBLE
  {
      $$ = "double"
  }

  | char_type
  {
      $$ = $1
  }

char_type: CHAR length_opt
  {
    $$ = "char(" + $2 + ")"
  }

length_opt: '(' NUMBER ')'
  {
     $$ = $2
  }

null_opt:
        {
            $$ = false
        }
        | NOT NULL
        {
            $$ = true
        }

column_key_opt:
              {
                  $$ = false
              }
              | KEY
              {
                $$ = true
              }
              | PRIMARY KEY
              {
                  $$ = true
              }

fragment_table: h_fragment_table
                   {
                        $$ = $1
                        setFragment(&($$))
                   }
              | v_fragment_table
                   {
                        $$ = $1
                        setFragment(&($$))
                   }
h_fragment_table: FRAGMENT table_name HORIZONTALLY INTO h_fragment_list
               {
                    $$ = Fragment{TableName: $2, FragmentMode: "H", FragmentNum: 0, FragmentSpec: &($5)}
               }

v_fragment_table: FRAGMENT table_name VERTICALLY INTO v_fragment_list  // 用大写的H和V表示水平和垂直划分
               {
                    $$ = Fragment{TableName: $2, FragmentMode: "V", FragmentNum: 0, FragmentSpec: &($5)}
               }

h_fragment_list: h_fragment
               {
                    // FIXME
                    temp := $1
                    $$.Fragments = append($$.Fragments, &(temp))
               }
               | h_fragment_list ',' h_fragment
               {
                  // FIXME
                    temp := $3
                    $$.Fragments = append($$.Fragments, &(temp))
               }

h_fragment: h_fragment_limit    //h_f 相当于一个fragment, f_l 相当于其中id>1000 f_l_op 相当于 and
          {
                $$.ColumnList = append($$.ColumnList, $1[0])   // 水平分片的条件属性赋给columnlist作为分片属性存储
                $$.Limit += $1[1]
          }
          | h_fragment fragment_l_op h_fragment_limit
          {
            $$.ColumnList = append($$.ColumnList, $3[0])
            $$.Limit += $2 + $3[1]
          }

h_fragment_limit: IDENT a_op NUMBER
               {    $$ = []string{"",""}
                    $$[0] = $1 // $$[0]
                    $$[1] = $1 + $2 + $3    // $$[1] 我们用用一个字符串切片存储 columnname 和 limit
               }

fragment_l_op : AND
   {
        $$ = " and "
   }
   | OR
   {
        $$ = " | "
   }
   |'&'
   {
        $$ = " & "
   }

a_op: '>' {$$ = ">"}
    | '=' {$$ = "="}
    | '<' {$$ = "<"}
    | '>' '=' {$$ = ">="}
    | '<' '=' {$$ = "<="}
    | '<' '>' {$$ = "<>"}

v_fragment_list: v_fragment
                {
                    $$.Fragments = append($$.Fragments, $1)
                }
               | v_fragment_list ',' v_fragment
               {
                    $$.Fragments = append($$.Fragments, $3)
               }


v_fragment: '(' v_frag_columns ')'
            {
                $$ = &FragmentDefinition{FragmentName: "", ColumnList: $2, Limit: "*"}
            }

v_frag_columns: column_name
            {
                $$ = append($$, $1)
            }
            | v_frag_columns ',' column_name
            {
                $$ = append($$, $3)
            }

allocate_frag: ALLOCATE NAMEWITHDOT TO IDENT
                {
                    $$ = Allocate{FragmentName: $2, Site: $4}
                }
insert_table: INSERT INTO table_name VALUES '(' insert_values ')'
    {
        $$ = Insert{TableName: $3, InsertMap: nil, InsertValues: $6 } // InsertColumns: 我们会在后面根据gdd进行更新map
    }

insert_values: insert_value
               {
                    $$ = append($$, $1)
               }
             | insert_values ',' insert_value
               {
                    $$ = append($$, $3)
               }

insert_value: NUMBER
              {
                    $$ = $1
              }
            | STRING
              {
                    $$ = $1
              }

delete_table: DELETE FROM table_name
            {
                $$ = Delete{TargetName: $3,Condition: "*",}
            }

          | DELETE FROM table_name WHERE delete_conditions
            {
               $$ = Delete{TargetName: $3, Condition: $5[1], DeleteCols: $5[0]}
            }

delete_conditions: delete_condition
                   {
                        $$ = append($$, $1[0])
                        $$ = append($$, $1[1])
                   }
                 | delete_conditions d_l_op delete_condition
                   {
                        $$[1] += $2 + $3[1]  // 同上， 使用$$ 和 $1 一样
                   	$$[0] += "," + $3[0] // 只加逗号不加空格 谢谢 :)
                   }

d_l_op: AND
        {
                $$ = " and "
        }
      | OR
        {
                $$ = " or "
        }

delete_condition: IDENT a_op NUMBER
            {
            	$$ = append($$, $1)   // for delete vertically table with condition where
                $$ = append($$, $1 + $2 +$3)

            }
            | IDENT a_op STRING
            {
                $$ = append($$, $1)   // for delete vertically table with condition where
                $$ = append($$, $1 + $2 +$3)
            }
load_stmt: LOAD table_name FROM STRING
            {
                $$ = Load{tableName: $2, filePath: $4}
            }
%%















