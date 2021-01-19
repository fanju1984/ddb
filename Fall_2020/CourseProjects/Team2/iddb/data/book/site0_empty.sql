drop database if exists iddb;
create database iddb;
use iddb;
drop table if exists publisher_0;
create table publisher_0(pid int, pname char(100), nation char(3));
drop table if exists book_0;
create table book_0(bid int, title char(100), authors char(200), bpid int, copies int);
drop table if exists customer_0;
create table customer_0(cid int, cname char(25));
drop table if exists orders_0;
create table orders_0(ocid int, obid int, quantity int);

alter table publisher_0 add primary key(pid);
alter table book_0 add primary key(bid);
alter table customer_0 add primary key(cid);
alter table orders_0 add primary key(ocid, obid, quantity);