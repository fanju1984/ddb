package storage

import (
	"context"
	"gDB/pb"
	pb "gDB/pb"
	"github.com/juju/errors"
	"google.golang.org/grpc"
	"strings"
)

type RPCClient struct {
	Name   string
	Client peerCommunication.PeerCommunicationClient
	Conn   *grpc.ClientConn
}

func initRPCClient(grpcPeers string) ([]RPCClient, error) {
	peers := strings.Split(strings.Trim(grpcPeers, " "), ",")
	var rpcClients []RPCClient
	for _, peer := range peers {
		factors := strings.Split(peer, "=")
		conn, err := grpc.Dial(factors[1], grpc.WithInsecure())
		if err != nil {
			return nil, errors.Trace(err)
		}
		client := pb.NewPeerCommunicationClient(conn)
		rpcClients = append(rpcClients, RPCClient{Name: strings.Trim(factors[0], " "), Client: client, Conn: conn})
	}
	return rpcClients, nil
}

func NotifyAddRPCClient(name string, grpcPeer string) error {
	for _, rpcClient := range Storage.rpcClients {
		rpcClient.Client.AddRPCClient(context.Background(), &pb.NewClient{Name: name, Url: grpcPeer})
	}
	return nil
}

func AddRPCClient(name string, grpcPeer string) error {
	conn, err := grpc.Dial(grpcPeer, grpc.WithInsecure())
	if err != nil {
		return errors.Trace(err)
	}
	client := pb.NewPeerCommunicationClient(conn)
	Storage.rpcClients = append(Storage.rpcClients, RPCClient{Name: name, Client: client, Conn: conn})
	return nil
}
