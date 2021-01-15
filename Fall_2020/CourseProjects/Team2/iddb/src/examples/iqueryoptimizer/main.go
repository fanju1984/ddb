package main

import (
	"iparser"
	"iplan"
	"iqueryanalyzer"
	"iqueryoptimizer"
)

func main() {
	//test_old()
	var pt iplan.PlanTree
	var sqlstmt string
	// sqlstmt := `select customer.name,orders.quantity
	// from customer,orders
	// where customer.id=orders.customer_id`
	//1
	// sqlstmt = `
	// select *
	// from customer`

	//2
	// sqlstmt = `
	// 	select publisher.name
	// 	from publisher`

	//3
	// sqlstmt = `
	// select book.title
	// from book
	// where copies>5000`

	//4
	// sqlstmt = `
	// select orders.customer_id,orders.quantity
	// from orders
	// where quantity<8`
	//5
	// sqlstmt = `
	// select book.title, book.copies, publisher.name, publisher.nation
	// from book, publisher
	// where book.publisher_id = publisher.id
	// and publisher.nation='USA'  //!!!!!!
	// and book.copies>1000`
	//6
	// sqlstmt = `
	// select customer.name, orders.quantity
	// from customer,orders
	// where customer.id=orders.customer_id`
	// //7
	// sqlstmt = `
	// select customer.name, customer.rank, orders.quantity
	// from customer,orders
	// where customer.id=orders.customer_id
	// and customer.rank=1`

	// 8
	// sqlstmt = `
	// select customer.name, orders.quantity, book.title
	// from customer,orders,book
	// where customer.id=orders.customer_id
	// and book.id=orders.book_id
	// and customer.rank=1
	// and book.copies>5000`

	// //9
	// sqlstmt = `
	// select customer.name, book.title, publisher.name, orders.quantity
	// from customer, book, publisher, orders
	// where customer.id=orders.customer_id
	// and book.id=orders.book_id
	// and book.publisher_id=publisher.id
	// and book.id>220000
	// and publisher.nation='USA'
	// and orders.quantity>1`

	//10
	// sqlstmt = `
	// select customer.name, book.title, publisher.name, orders.quantity
	// from customer, book, publisher, orders
	// where customer.id=orders.customer_id
	// and book.id=orders.book_id
	// and book.publisher_id=publisher.id
	// and customer.id>308000
	// and book.copies>100
	// and orders.quantity>1
	// and publisher.nation='PRC'`

	sqlstmt = `select customer.name, book.title, publisher.name, orders.quantity 
	from customer, book, publisher, orders 
	where customer.id=orders.customer_id 
	and book.id=orders.book_id 
	and book.publisher_id=publisher.id 
	and book.id > 207000 
	and book.id < 213000 
	and book.copies>100 
	and orders.quantity>1 
	and publisher.nation='PRC'`

	pt = iparser.Parse(sqlstmt, 1)
	pt = iqueryanalyzer.Analyze(pt)
	pt = iqueryoptimizer.Optimize(pt)
	// pt.Print()gn[[]]
	return
}
