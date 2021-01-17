select *
from Customer;

select Publisher.name
from Publisher;

select Book.title
from Book
where copies > 5000;

select customer_id, quantity
from Orders
where quantity < 8;

select Book.title, Book.copies, Publisher.name, Publisher.nation
from Book,Publisher
where Book.publisher_id = Publisher.id
and Publisher.nation = 'USA'
and Book.copies > 1000;

select Customer.name, Orders.quantity
from Customer, Orders
where Customer.id = Orders.customer_id;

select Customer.name, Customer.rank, Orders.quantity
from Customer, Orders
where Customer.id = Orders.customer_id
and Customer.rank = 1;

select Customer.name, Orders.quantity, Book.title
from Customer, Orders, Book
where Customer.id = Orders.customer_id
and Book.id = Orders.book_id 
and Customer.rank = 1
and Book.copies > 5000;

select Customer.name, Book.title, Publisher.name, Orders.quantity
from Customer, Book, Publisher, Orders
where Customer.id = Orders.customer_id
and Book.id = Orders.book_id
and Book.publisher_id = Publisher.id
and Book.id > 220000
and Publisher.nation = 'USA'
and Orders.quantity > 1;

select Customer.name, Book.title, Publisher.name, Orders.quantity
from Customer, Book, Publisher, Orders
where Customer.id = Orders.customer_id
and Book.id = Orders.book_id
and Book.publisher_id = Publisher.id
and Customer.id > 308000
and Book.copies > 100
and Orders.quantity > 1
and Publisher.nation = 'PRC';

show tables;
insert into Customer(id, name, rank) values(300001, 'Xiaoming', 1);
insert into Publisher(id, name, nation) values(104001,'High Education Press', 'PRC');
insert into Customer(id, name, rank) values(300002,'Xiaohong', 1);
insert into Book(id, title, authors, publisher_id, copies) values(205001, 'DDB', 'Oszu', 104001, 100);
insert into Orders(customer_id, book_id, quantity) values(300001, 205001,5);

Delete from Orders;
Delete from Book where copies = 100;
delete from Publisher where nation = 'PRC';
delete from Customer where name='Xiaohong' AND rank=1;
delete from Customer where rank = 1;
