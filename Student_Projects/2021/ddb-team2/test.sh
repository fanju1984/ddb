# start server
./scheduler --server_init=true
./scheduler --server_init=true
./scheduler --port=7999 --server_init=true
./scheduler --port=7997 --server_init=true

./scheduler --port=8000 --server_init=false
./scheduler --port=7999 --server_init=false
./scheduler --port=7998 --server_init=false
./scheduler --port=7997 --server_init=false

# test insert
./service --init_all=true --test_insert=true --test_sql=false

# test delete
./service --init_all=false --test_delete=true --test_sql=false

#1
./service --init_all=true --load=true  --sql="select * from customer where customer.id > 0;" --verbose=false

#2
./service --init_all=false --sql="select publisher.name from publisher where publisher.id > 0;" --verbose=false

#3
./service --init_all=false --sql="select book.title from book where book.copies>5000;" --verbose=false

#4
./service --init_all=false --sql="select orders.customer_id, orders.quantity from orders where orders.quantity < 8;" --verbose=false

#5
./service --init_all=false --sql="select book.title,book.copies, publisher.name,publisher.nation from book,publisher where book.publisher_id=publisher.id and publisher.nation='USA' and book.copies > 1000;" --verbose=true

#6
./service --init_all=false --sql="select customer.name,orders.quantity from customer,orders where customer.id=orders.customer_id;" --verbose=false

#7
./service --init_all=false --sql="select customer.name,customer.rank,orders.quantity from customer,orders where customer.id=orders.customer_id and customer.rank=1;" --verbose=true

#8
./service --init_all=false --sql="select customer.name ,orders.quantity,book.title from customer,orders,book where customer.id=orders.customer_id and book.id=orders.book_id  and customer.rank=1 and book.copies>5000;" --verbose=false

#9
./service --init_all=false --sql="select customer.name, book.title, publisher.name, orders.quantity from customer, book, publisher, orders where customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and book.id>220000 and publisher.nation='USA' and orders.quantity>1;" --verbose=false

#10
./service --init_all=false --sql="select customer.name, book.title,publisher.name, orders.quantity from customer, book, publisher, orders where customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and customer.id>308000 and book.copies>100 and orders.quantity>1 and publisher.nation='PRC';" --verbose=false

