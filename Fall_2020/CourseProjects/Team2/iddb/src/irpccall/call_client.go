/*
call_client
Author: Kunyao Wu
the implementation of irpc executer caller client
*/
package irpccall

import (
	"context"
	"iutilities"
	"log"
	"time"

	"google.golang.org/grpc"
)

/*
Call a remote site to work (execute as plan)
input:
	caddress: executer call target address
	txnid: transaction id
output:
	1 for succeed, 0 for failed
*/
func RunCallClient(caddress string, txnid int64) int64 {
	defer iutilities.Waitgroup.Done()
	println("rpccall client", caddress)
	// Set up a connection to the server.
	conn, err := grpc.Dial(caddress, grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := NewExecuterCallerClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*30000)

	defer cancel()
	irpccallreq := IrpcCallReq{Txnid: txnid}
	r, err := c.ExecuterCall(ctx, &irpccallreq)
	if err != nil {
		log.Fatalf("could not call: %v", err)
	}
	return r.IsSuc
}
