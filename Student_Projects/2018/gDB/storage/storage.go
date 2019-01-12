package storage

import (
	"github.com/juju/errors"
	log "github.com/sirupsen/logrus"
)

type storage struct {
	rpcClients []RPCClient

	DB Database
}

var Storage storage

func InitDB(port int, info ...string) {
	Storage.DB = Database{
		PGUsername: info[0],
		PGPassword: info[1],
		PGBinPath:  info[2],
		PGDataPath: info[3],
		PGPort:     port,
	}
}

func InitRPCClient(grpcPeers string) {
	var err error
	if Storage.rpcClients, err = initRPCClient(grpcPeers); err != nil {
		log.Fatalf("cannot connect to rpc peers")
	}
}

func CloseRPCClient() {
	for _, rpcClient := range Storage.rpcClients {
		rpcClient.Conn.Close()
	}
}

func (s *storage) GetRPCClient(name string) (*RPCClient, error) {
	for _, c := range s.rpcClients {
		if c.Name == name {
			return &c, nil
		}
	}
	return nil, errors.New("No such client")
}

func StopDB() error {
	return Storage.DB.StopDatabase()
}

func StartDB() error {
	return Storage.DB.StartDatabase()
}
