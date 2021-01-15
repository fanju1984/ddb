package main

// func main() {
// 	runtime.GOMAXPROCS(1)
// 	go executor(1)
// 	go executor(2)
// 	go executor(3)
// 	go executor(4)
// 	for j := 1; ; {
// 		j++
// 		j--
// 	}
// }

// func executor(siteid int64) bool {

// 	seed := rand.NewSource(time.Now().UnixNano())
// 	rng := rand.New(seed)
// 	for i := 0; i < 10; i++ {
// 		time.Sleep(time.Duration(time.Second) * time.Duration(rng.Intn(3)+1))
// 		println("siteid=", siteid, ",i=", i)
// 	}
// 	return true
// }


