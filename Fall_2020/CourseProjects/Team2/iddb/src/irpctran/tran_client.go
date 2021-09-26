/*
tran_client
Author: Kunyao Wu
the implementation of irpc transmission client
*/
package irpctran

import (
	"context"
	"log"
	"time"

	"google.golang.org/grpc"
)

/*
Push table from current site to target site
input:
	taddress: transmission address of target site,
		such as "localhost:50053" or "10.77.70.162:50053"
	table: struct Table defined in irpc.pb.go
output:
	1 for succeed, 0 for failed
*/
func RunTranClient(taddress string, table Table) int64 {
	println("irpctran started")
	// Set up a connection to the server.
	conn, err := grpc.Dial(taddress, grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := NewPushTableClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*30000)
	defer cancel()

	r, err := c.PushTable(ctx, &table)
	if err != nil {
		log.Fatalf("could not call: %v", err)
	}
	println("transmission suc ")
	return r.IsSuc
}
