drop database if exists iddb;
create database iddb;
use iddb;
drop table if exists publisher_2;
create table publisher_2(pid int, pname char(100), nation char(3));
drop table if exists book_2;
create table book_2(bid int, title char(100), authors char(200), bpid int, copies int);
drop table if exists orders_2;
create table orders_2(ocid int, obid int, quantity int);

alter table publisher_2 add primary key(pid);
alter table book_2 add primary key(bid);
alter table orders_2 add primary key(ocid, obid, quantity);
