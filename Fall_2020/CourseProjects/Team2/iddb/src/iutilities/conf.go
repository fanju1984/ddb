package iutilities

import (
	"fmt"

	"github.com/toml"
)

var (
	Configfile = "/home/centos/config/iddb.conf"

// Configfile = "D:/code/GoProject/iddb/cnf/iddb.conf"
)

var (
	Me    Nodes
	Peers []Nodes
	Mysql MysqlConfig
)

type MysqlConfig struct {
	Mysql_user    string
	Mysql_passwd  string
	Mysql_db      string
	Mysql_ip_port string
}
type tomlConfig struct {
	NodeId        int64
	Mysql_user    string
	Mysql_passwd  string
	Mysql_db      string
	Mysql_ip_port string
	Cluster       map[string]Nodes
}

type Nodes struct {
	NodeId int64
	IP     string
	Tran   string
	Call   string
}

func LoadAllConfig() {
	Me = GetMe()
	// PrintMe()
	Peers = GetPeers()
	// PrintPeers()
	Mysql = GetMysqlConfig()
	// PrintMysqlConfig()
	return
}

func (n *Nodes) Print() {
	println("NodeId= ", n.NodeId)
	println("IP= ", n.IP)
	println("Tran= ", n.Tran)
	println("Call= ", n.Call)
}

func getMe(config tomlConfig) Nodes {
	for _, node := range config.Cluster {
		if node.NodeId == config.NodeId {
			return node
		}
	}
	var node Nodes
	return node
}

func getPeers(config tomlConfig) []Nodes {
	peers := make([]Nodes, 4)
	for _, node := range config.Cluster {
		i := node.NodeId
		peers[i] = node
	}

	return peers
}

func PrintPeers() {
	for _, node := range Peers {
		node.Print()
	}
	return
}

func getMysqlConfig(config tomlConfig) MysqlConfig {
	var mysql MysqlConfig
	mysql.Mysql_user = config.Mysql_user
	mysql.Mysql_passwd = config.Mysql_passwd
	mysql.Mysql_db = config.Mysql_db
	mysql.Mysql_ip_port = config.Mysql_ip_port
	return mysql
}

func printMe(config tomlConfig) {
	node := getMe(config)
	node.Print()
	return
}

func PrintMe() {
	var config tomlConfig
	if _, err := toml.DecodeFile(Configfile, &config); err != nil {
		fmt.Println(err)
		return
	}
	printMe(config)

}

func printMysqlConfig(config tomlConfig) {
	mysql := getMysqlConfig(config)
	println("mysql_user= ", mysql.Mysql_user)
	println("mysql_passwd= ", mysql.Mysql_passwd)
	println("mysql_db= ", mysql.Mysql_db)
	println("mysql_ip_port= ", mysql.Mysql_ip_port)
	return
}

func PrintMysqlConfig() {
	var config tomlConfig
	if _, err := toml.DecodeFile(Configfile, &config); err != nil {
		fmt.Println(err)
		return
	}
	printMysqlConfig(config)

}

func GetMe() Nodes {
	var config tomlConfig
	if _, err := toml.DecodeFile(Configfile, &config); err != nil {
		fmt.Println(err)
	}
	return getMe(config)
}

func GetPeers() []Nodes {
	var config tomlConfig
	if _, err := toml.DecodeFile(Configfile, &config); err != nil {
		fmt.Println(err)
	}
	return getPeers(config)
}

func GetMysqlConfig() MysqlConfig {
	var config tomlConfig
	if _, err := toml.DecodeFile(Configfile, &config); err != nil {
		fmt.Println(err)
	}
	return getMysqlConfig(config)
}
