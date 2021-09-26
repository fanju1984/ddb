use iddb
drop table if exists publisher;
create table publisher(pid int primary key, pname char(100), nation char(3));
drop table if exists book;
create table book(bid int primary key, title char(100), authors char(200), bpid int, copies int);
drop table if exists customer;
create table customer(cid int primary key, cname char(25), rank int);
drop table if exists orders;
create table orders(ocid int, obid int, quantity int);

LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/publisher.tsv'
INTO TABLE iddb.publisher;
LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/book.tsv'
INTO TABLE iddb.book;
LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/customer.tsv'
INTO TABLE iddb.customer;
LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/order.tsv'
INTO TABLE iddb.orders;

/*site 0*/
drop table if exists publisher_0;
create table publisher_0 select * from publisher where pid < 104000 and nation = 'PRC';
drop table if exists book_0;
create table book_0 select * from book where bid < 2050000;
drop table if exists customer_0;
create table customer_0 select cid,cname from customer;
drop table if exists orders_0;
create table orders_0 select * from orders where ocid < 307000 and obid < 215000;
alter table publisher_0 add primary key(pid);
alter table book_0 add primary key(bid);
alter table customer_0 add primary key(cid);
alter table orders_0 add primary key(ocid, obid, quantity);
/*site 1*/
drop table if exists publisher_1;
create table publisher_1 select * from publisher where pid < 104000 and nation = 'USA';
drop table if exists book_1;
create table book_1 select * from book where bid >= 2050000 and bid < 210000;
drop table if exists customer_1;
create table customer_1 select cid,rank from customer;
drop table if exists orders_1;
create table orders_1 select * from orders where ocid < 307000 and obid >= 215000;
alter table publisher_1 add primary key(pid);
alter table book_1 add primary key(bid);
alter table customer_1 add primary key(cid);
alter table orders_1 add primary key(ocid, obid, quantity);
/*site 2*/
drop table if exists publisher_2;
create table publisher_2 select * from publisher where pid >= 104000 and nation = 'PRC';
drop table if exists book_2;
create table book_2 select * from book where bid >= 2050000 and bid >= 210000;
drop table if exists orders_2;
create table orders_2 select * from orders where ocid >= 307000 and obid < 215000;
alter table publisher_2 add primary key(pid);
alter table book_2 add primary key(bid);
alter table orders_2 add primary key(ocid, obid, quantity);
/*site 3*/
drop table if exists publisher_3;
create table publisher_3 select * from publisher where pid >= 104000 and nation = 'USA';
drop table if exists orders_3;
create table orders_3 select * from orders where ocid >= 307000 and obid >= 215000;
alter table publisher_3 add primary key(pid);
alter table orders_3 add primary key(ocid, obid, quantity);