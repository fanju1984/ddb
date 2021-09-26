drop database if exists iddb;
create database iddb;
use iddb;
drop table if exists publisher_1;
create table publisher_1(pid int, pname char(100), nation char(3));
drop table if exists book_1;
create table book_1(bid int, title char(100), authors char(200), bpid int, copies int);
drop table if exists customer_1;
create table customer_1(cid int, rank int);
drop table if exists orders_1;
create table orders_1(ocid int, obid int, quantity int);

alter table publisher_1 add primary key(pid);
alter table book_1 add primary key(bid);
alter table customer_1 add primary key(cid);
alter table orders_1 add primary key(ocid, obid, quantity);
