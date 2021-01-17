use test;
    DROP TABLE IF EXISTS users;
        CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL,
  `name` varchar(20) DEFAULT NULL,
  `fullname` varchar(40) DEFAULT NULL,
  `password` varchar(12) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(3, 'mary', 'Mary Contrary', 'xxg527');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(6, 'fred', 'Fred Flinstone', 'blah');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(9, 'jack', 'Jack Bean', 'gjffdd');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(12, 'mary', 'Mary Contrary', '123');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(15, 'fred', 'Fred Flinstone', 'blah');
INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES(18, 'wxw02', 'Jack wxw2', '234');
