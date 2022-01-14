define china 10.77.70.61 8883
define japan 10.77.70.61 8885
define england 10.77.70.62 8883
define usa 10.77.70.63 8883
define britain 10.77.70.62 8883

create table Publisher (id int key, name char(100), nation char(3));
create table Book (id int key, title char(100), authors char(200), publisher_id int, copies int);
create table Customer (id int key, name char(25), rank int);
create table Orders (customer_id int, book_id int, quantity int);

create fragment Publisher(*) where id < 104000 and nation='PRC' on site='china';
create fragment Publisher(*) where id < 104000 and nation='USA' on site='japan';
create fragment Publisher(*) where id >= 104000 and nation='PRC' on site='england';
create fragment Publisher(*) where id >= 104000 and nation='USA' on site='usa';

create fragment Book(*) where id < 205000 on site='china';
create fragment Book(*) where id >= 205000 and id < 210000 on site='japan';
create fragment Book(*) where id >= 210000 on site='england';

create fragment Customer(id, name) on site='china';
create fragment Customer(id, rank) where rank < 2 on site='japan';
create fragment Customer(id, rank) where rank >= 2 on site='usa';




create fragment Customer(id, name) on site='china';
create fragment Customer(id, rank) on site='japan';

create fragment Orders(*) where customer_id < 307000 and book_id < 215000 on site='china';
create fragment Orders(*) where customer_id < 307000 and book_id >= 215000 on site='japan';
create fragment Orders(*) where customer_id >= 307000 and book_id < 215000 on site='england';
create fragment Orders(*) where customer_id >= 307000 and book_id >= 215000 on site='usa';

load data infile "/home/centos/DDB_RPQL/data/publisher.tsv" into table Publisher;
load data infile "/home/centos/DDB_RPQL/data/book.tsv" into table Book;
load data infile "/home/centos/DDB_RPQL/data/customer.tsv" into table Customer;
load data infile "/home/centos/DDB_RPQL/data/orders.tsv" into table Orders;

delete from Publisher;
delete from Book;
delete from Orders;
delete from Customer;

drop table Publisher;
drop table Orders;
drop table Book;
drop table Customer;

insert into Customer(id, name, rank) values(300001, 'Xiaoming', 1), (300002,'Xiaohong', 1);
insert into Publisher(id, name, nation) values(104001,'High Education Press', 'PRC');
insert into Book(id, title, authors, publisher_id, copies) values(205001, 'DDB', 'Oszu', 104001, 100);
insert into Orders (customer_id, book_id, quantity) values(300001, 205001,5);
delete from Orders;
delete from Book where copies = 100;
delete from Publisher where nation = 'PRC';
delete from Customer where name='Xiaohong' AND rank=1;
delete from Customer where rank = 1;

add info Publisher.id U 100001 105000 5000
add info Publisher.nation P 'USA' 0.5 'PRC' 0.5
add info Customer.id U 300001 315000 15000
add info Customer.rank P 1 0.4 2 0.3 3 0.3
add info Book.id U 200001 250000 50000
add info Book.copies U 0 10000 50000
add info Orders.quantity N 3 2 1 12


