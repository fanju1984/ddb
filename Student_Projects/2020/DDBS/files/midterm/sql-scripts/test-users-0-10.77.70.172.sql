use test;
    DROP TABLE IF EXISTS users;
        CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL,
  `name` varchar(20) DEFAULT NULL,
  `fullname` varchar(40) DEFAULT NULL,
  `password` varchar(12) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(1, 'ed', 'Ed Jones', 'edspassword');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(4, 'lisa', 'lisa Contrary', 'ls123');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(7, 'jack', 'Jack Bean', 'gjffdd');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(10, 'ed', 'Ed Jones', '888');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(13, 'lisa', 'lisa Contrary', 'ls123');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(16, 'jack', 'Jack Bean', 'gjffdd');
