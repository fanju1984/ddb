package main

import (
	"fmt"
	"sync"
)

func main() {
	var waitGroup sync.WaitGroup
	waitGroup.Add(6)
	for i:=0; i< 6; i++{
		go func() {
			defer waitGroup.Done()
			fmt.Println("haha")
		}()
	}
	waitGroup.Wait()
	fmt.Println("executed.")
}
