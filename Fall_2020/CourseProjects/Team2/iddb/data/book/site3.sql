drop database if exists iddb2;
create database iddb2;
use iddb2;
drop table if exists publisher;
create table publisher(pid int primary key, pname char(100), nation char(3));
drop table if exists book;
create table book(bid int primary key, title char(100), authors char(200), bpid int, copies int);
drop table if exists customer;
create table customer(cid int primary key, cname char(25), rank int);
drop table if exists orders;
create table orders(ocid int, obid int, quantity int);

LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/publisher.tsv'
INTO TABLE iddb2.publisher;
LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/book.tsv'
INTO TABLE iddb2.book;
LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/customer.tsv'
INTO TABLE iddb2.customer;
LOAD DATA LOCAL INFILE '/home/centos/iddb/data/book/order.tsv'
INTO TABLE iddb2.orders;

/*site 3*/
drop table if exists publisher_3;
create table publisher_3 select * from publisher where pid >= 104000 and nation = 'USA';
drop table if exists orders_3;
create table orders_3 select * from orders where ocid >= 307000 and obid >= 215000;
alter table publisher_3 add primary key(pid);
