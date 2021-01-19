/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2020/11/18 10:09:54                          */
/*==============================================================*/


drop table if exists Book;

drop table if exists Customer;

drop table if exists Orders;

drop table if exists Publisher;

/*==============================================================*/
/* Table: Book                                                  */
/*==============================================================*/
create table Book
(
   id                   int not null,
   title                char(100),
   authors              char(200),
   copies               int,
   publisher_id         int not null,
   primary key (id)
);

/*==============================================================*/
/* Table: Customer                                              */
/*==============================================================*/
create table Customer
(
   id                   int not null auto_increment,
   name                 char(25),
   rank                 int,
   primary key (id)
);

/*==============================================================*/
/* Table: Orders                                                */
/*==============================================================*/
create table Orders
(
   customer_id          int not null,
   book_id              int not null,
   quantity             int,
   primary key (customer_id, book_id)
);

/*==============================================================*/
/* Table: Publisher                                             */
/*==============================================================*/
create table Publisher
(
   id                   int not null auto_increment,
   name                 char(100),
   nation               char(3),
   primary key (id)
);

alter table Book add constraint FK_BOOK_REFRENCE_PUBLISHER foreign key (publisher_id)
      references Publisher (id) on delete restrict on update restrict;

alter table Orders add constraint FK_ORDERS_REFRENCE_ORDERS foreign key (book_id)
      references Book (id) on delete restrict on update restrict;

alter table Orders add constraint FK_ORDERS_REFRENCE_CUSTOMER foreign key (customer_id)
      references Customer (id) on delete restrict on update restrict;

