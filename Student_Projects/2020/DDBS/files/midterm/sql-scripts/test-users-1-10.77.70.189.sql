use test;
    DROP TABLE IF EXISTS users;
        CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL,
  `name` varchar(20) DEFAULT NULL,
  `fullname` varchar(40) DEFAULT NULL,
  `password` varchar(12) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(2, 'wendy', 'Wendy Williams', 'foobar');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(5, 'cred', 'cred Flinstone', 'bla123');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(8, 'ed', 'Ed Jones', 'edspassword');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(11, 'wendy', 'Wendy Williams', 'foobar');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(14, 'cred', 'cred Flinstone', 'bla123');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(17, 'wxw01', 'Jack wxw', '123');
