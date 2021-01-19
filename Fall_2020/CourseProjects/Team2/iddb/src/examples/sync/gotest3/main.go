package main

import (
	"fmt"
	"runtime"
	_ "time"
)

var (
	flag = false
	str  string
)

func foo() {
	flag = true
	str = "setup complete!"
}

func main() {
	runtime.GOMAXPROCS(8)
	go foo()
	//time.Sleep(1 * time.Second)
	for {
		if flag {
			break
		}
	}
	fmt.Println(str)
}
