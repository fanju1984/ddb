package main

import (
	"bufio"
	"context"
	"flag"
	"fmt"
	"gDB/SQL"
	"gDB/dataManager"
	"gDB/server"
	"github.com/juju/errors"
	log "github.com/sirupsen/logrus"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"
)

func main() {
	cfg := server.NewConfig()
	err := cfg.Parse(os.Args[1:])

	switch errors.Cause(err) {
	case nil:
	case flag.ErrHelp:
		os.Exit(0)
	default:
		log.Fatalf("parse cmd flags error: %s\n", errors.ErrorStack(err))
	}

	svr, err := server.CreateServer(cfg)
	if err != nil {
		log.Fatalf("create server failed: %v", errors.ErrorStack(err))
	}

	sc := make(chan os.Signal, 1)
	signal.Notify(sc,
		syscall.SIGHUP,  //1
		syscall.SIGINT,  //2
		syscall.SIGTERM, //f
		syscall.SIGQUIT) //3

	ctx, cancel := context.WithCancel(context.Background())
	var sig os.Signal
	go func() {
		sig = <-sc
		cancel()
	}()

	if err := svr.Run(ctx); err != nil {
		log.Fatalf("run server failed: %v", errors.ErrorStack(err))
	}

	go svr.StartRPC()

	inputReader := bufio.NewReader(os.Stdin)
	go func() {
		for {
			fmt.Print("gDB>")
			input, err := inputReader.ReadString('\n')
			if err != nil {
				log.Errorf("Read input error: ", errors.Trace(err))
			}
			lowerInput := strings.TrimLeft(strings.ToLower(input), " ")
			if strings.Index(lowerInput, "select") == 0 {
				//TODO: Select function here
				fmt.Println("Select Now!!!")
				str := strings.Replace(input, "\n", "", -1)
				SQL.Test(str)
			} else if strings.Index(lowerInput, "add") == 0 {
				strs := strings.Split(strings.TrimRight(input, "\n"), " ")
				if err := svr.AddEtcd(strs[1], strs[2]); err != nil {
					log.Error("Add member error.")
				}
				//if err := storage.NotifyAddRPCClient(strs[1], strs[3]); err != nil {
				//	log.Error("Add member error")
				//}
			} else if input == "`" {
				//do nothing.
			} else {
				timeBefore := time.Now()
				stmt, err := dataManager.Parse(strings.NewReader(lowerInput))
				if err != nil {
					log.Error(err)
				}
					//stmt.PrintStmt()
					err = stmt.ExecStmt()
					if err != nil {
						//log.Println(err)
						fmt.Println(err)
					}
				//fmt.Println("Executed.")
				execTime := time.Since(timeBefore)
				fmt.Printf("执行时间:%v\n", execTime)
			}
		}
	}()

	<-ctx.Done()
	log.Infof("Got signal [%d] to exit.", sig)

	svr.StopRPC()
	svr.Close()

	switch sig {
	case syscall.SIGTERM:
		log.Infof("Quit with exit code 0")
		os.Exit(0)
	default:
		log.Infof("Quit with exit code 1")
		os.Exit(1)
	}
}
