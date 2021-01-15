/*
tran_server
Author: Kunyao Wu
the implementation of irpc transmission server
*/
package irpctran

import (
	"context"
	"itrans"
	"iutilities"
	"log"
	"net"
	"time"

	"google.golang.org/grpc"
)

var (
	tport string
)

type tserver struct {
	UnimplementedPushTableServer
}

/*
the implementation of PushTable that defined in irpc.pb.go
receive a table from remote client, make it a INSERT statement
and execute
*/
func (s *tserver) PushTable(ctx context.Context, in *Table) (*IrpcStatus, error) {
	now := time.Now()
	println("transmission started")
	createstmt := in.Createstmt
	itrans.ExecuteCreateStmt(createstmt)
	// println(in.Createstmt, "time cost:")
	println("transmission time cost=", time.Since(now).Milliseconds(), "ms")
	return &IrpcStatus{IsSuc: 1}, nil
}

/*
Regitering transmission server
*/
func RunTranServer() {
	// iutilities.LoadAllConfig()
	tport = ":" + iutilities.Me.Tran
	println("irpctran")
	lis, err := net.Listen("tcp", tport)
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	log.Printf("Listening tport %v **************", tport)
	s := grpc.NewServer()
	RegisterPushTableServer(s, &tserver{})
	log.Printf("Server Registering Successfully ***********")
	if err := s.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
