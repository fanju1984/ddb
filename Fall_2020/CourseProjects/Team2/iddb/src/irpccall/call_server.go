/*
call_server
Author: Kunyao Wu
the implementation of irpc executer caller server
*/
package irpccall

import (
	"context"
	"iexec"
	"iutilities"
	"log"
	"net"
	"time"

	"google.golang.org/grpc"
)

type cserver struct {
	UnimplementedExecuterCallerServer
}

var (
	cport string
)

// ExecuterCall implementation, defined in irpc.pb.go
func (s *cserver) ExecuterCall(ctx context.Context, in *IrpcCallReq) (*IrpcStatus, error) {
	now := time.Now()
	iexec.RunExecuter(in.Txnid)
	println(in.Txnid, "time cost:")
	println(time.Since(now).Milliseconds(), "ms")
	return &IrpcStatus{IsSuc: 1}, nil
}

func RunCallServer() {
	// iutilities.LoadAllConfig()
	cport = ":" + iutilities.Me.Call
	println("rpccall", iutilities.Me.NodeId)
	lis, err := net.Listen("tcp", cport)
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	log.Printf("Listening cport %v **************", cport)
	s := grpc.NewServer()
	RegisterExecuterCallerServer(s, &cserver{})
	log.Printf("Server Registering Successfully ***********")
	if err := s.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
