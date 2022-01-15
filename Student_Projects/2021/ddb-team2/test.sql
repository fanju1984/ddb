# create table
create table `book` (`id` int not null, `title` varchar(100) not null, `authors` varchar(200) not null, `publisher_id` int not null, `copies` int not null, primary key(`id`));
create table `customer` (`id` int, `name` varchar(25), `rank` int, primary key(`id`));
create table `orders` (`customer_id` int, `book_id` int, `quantity` int);
create table `publisher` (`id` int key, `name` varchar(100), `nation` varchar(3), primary key(`id`));

# insert
insert into customer(id, name, rank) values(300001, 'Xiaoming', 1);
insert into Book (id, title, authors, publisher_id, copies) values(205001, 'DDB', 'Oszu', 104001, 100);

# check
select count(*) from test_8000.book;
select count(*) from test_7999.book;
select count(*) from test_7998.book;
select count(*) from test_7997.book;

select count(*) from test_8000.pulisher;
select count(*) from test_7999.pulisher;
select count(*) from test_7998.pulisher;
select count(*) from test_7997.pulisher;

select count(*) from test_8000.orders;
select count(*) from test_7999.orders;
select count(*) from test_7998.orders;
select count(*) from test_7997.orders;

select count(*) from test_8000.customer;
select count(*) from test_7999.customer;
select count(*) from test_7998.customer;
select count(*) from test_7997.customer;


# 1
select * from customer;
select * from customer where customer.id > 0;
select * from customer where customer.rank > 0;

# 2
select publisher.name from publisher where publisher.id > 0;

# 3
select book.title from book where copies>5000;


# 4
select orders.customer_id, orders.quantity from orders where orders.quantity < 8;

# 5
select book.title,book.copies, publisher.name,publisher.nation from book,publisher where book.publisher_id=publisher.id and publisher.nation='USA' and book.copies > 1000;

# 6
select customer.name,orders.quantity from customer,orders where customer.id=orders.customer_id;

# 7
select customer.name,customer.rank,orders.quantity from customer,orders where customer.id=orders.customer_id and customer.rank=1;

# 8
select customer.name ,orders.quantity,book.title from customer,orders,book where customer.id=orders.customer_id and book.id=orders.book_id  and customer.rank=1 and book.copies>5000;

# 9 
select customer.name, book.title, publisher.name, orders.quantity from customer, book, publisher, orders where customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and book.id>220000 and publisher.nation='USA' and orders.quantity>1;

# 10
select customer.name, book.title,publisher.name, orders.quantity from customer, book, publisher, orders where customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and customer.id>308000 and book.copies>100 and orders.quantity>1 and publisher.nation='PRC';