package main

import "time"

func fuck(c chan string) {
	println("run1")
	a := "hello"
	time.Sleep(time.Duration(3) * time.Second)
	c <- a
	time.Sleep(time.Duration(10) * time.Second)
	println("run2")
}

func main() {
	c := make(chan string)
	go fuck(c)
	b := <-c
	println(b)
	time.Sleep(time.Duration(10) * time.Second)
}
