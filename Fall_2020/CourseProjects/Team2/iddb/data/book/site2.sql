drop database if exists iddb;
create database iddb;
use iddb;
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

/*site 2*/
drop table if exists publisher_2;
create table publisher_2 select * from publisher where pid >= 104000 and nation = 'PRC';
drop table if exists book_2;
create table book_2 select * from book where bid >= 210000;
drop table if exists orders_2;
create table orders_2 select * from orders where ocid >= 307000 and obid < 215000;
alter table publisher_2 add primary key(pid);
alter table book_2 add primary key(bid);
