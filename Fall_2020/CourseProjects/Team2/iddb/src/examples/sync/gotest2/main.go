package main

import (
	"fmt"
	"runtime"
	"sync"
	"time"
)

//定义任务队列
var waitgroup sync.WaitGroup

func xtgxiso(num int) {
	for i := 1; i <= 1000000000; i++ {
		num = num + i
		num = num - i
		num = num * i
		num = num / i
	}
	waitgroup.Done() //任务完成，将任务队列中的任务数量-1，其实.Done就是.Add(-1)
}

func main() {
	//记录开始时间
	start := time.Now()
	//设置最大的可同时使用的CPU核数和实际cpu核数一致
	runtime.GOMAXPROCS(2)
	for i := 1; i <= 10; i++ {
		waitgroup.Add(1) //每创建一个goroutine，就把任务队列中任务的数量+1
		go xtgxiso(i)
	}
	waitgroup.Wait() //Wait()这里会发生阻塞，直到队列中所有的任务结束就会解除阻塞 //记录结束时间
	end := time.Now()
	//输出执行时间，单位为秒。
	fmt.Println(end.Sub(start).Seconds())
}
