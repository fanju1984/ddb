drop database if exists iddb2;
create database iddb2;
use iddb2;
drop table if exists publisher_3;
create table publisher_3(pid int, pname char(100), nation char(3));
drop table if exists orders_3;
create table orders_3(ocid int, obid int, quantity int);

alter table publisher_3 add primary key(pid);
alter table orders_3 add primary key(ocid, obid, quantity);