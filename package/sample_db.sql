# CoLTE Sample Config Database
# Author: Spencer Sevilla
# License: MIT

DROP TABLE IF EXISTS `static_ips`;
CREATE TABLE `static_ips` (
  `imsi` varchar(16) NOT NULL,
  `ip` varchar(16) NOT NULL,
  PRIMARY KEY (`imsi`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `static_ips` VALUES
('000000000000001', '127.0.0.1'),
('001010123456789', '192.168.151.2'),
('208920100001100', '192.168.151.3'),
('208920100001101', '192.168.151.4'),
('208920100001102', '192.168.151.5'),
('208920100001103', '192.168.151.6'),
('208920100001104', '192.168.151.7'),
('208920100001105', '192.168.151.8'),
('208920100001106', '192.168.151.9'),
('208920100001107', '192.168.151.10'),
('208920100001108', '192.168.151.11'),
('208920100001109', '192.168.151.12'),
('208920100001111', '192.168.151.13')
;

DROP TABLE IF EXISTS `customers`;
CREATE TABLE `customers` (
  `imsi` varchar(16) NOT NULL,
  `username` varchar(50) DEFAULT NULL,
  `raw_down` bigint(15) unsigned DEFAULT '0',
  `raw_up` bigint(15) unsigned DEFAULT '0',
  `data_balance` bigint(15) DEFAULT '10000000',
  `balance` decimal(13,4) DEFAULT '0' COMMENT 'this value is currency-less',
  `bridged` tinyint(1) DEFAULT '1',
  `enabled` tinyint(1) DEFAULT '1',
  `admin` tinyint(1) DEFAULT '0',
  `msisdn` varchar(16) NOT NULL,
  PRIMARY KEY (`imsi`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `customers` VALUES
('000000000000001', 'User1', 0, 0, 10000000, 0, 1, 1, 1, '1'),
('001010123456789', 'User2', 0, 0, 10000000, 0, 1, 1, 0, '2'),
('208920100001100', 'User3', 0, 0, 10000000, 0, 1, 1, 0, '3'),
('208920100001101', 'User4', 0, 0, 10000000, 0, 1, 1, 0, '4'),
('208920100001102', 'User5', 0, 0, 10000000, 0, 1, 1, 0, '5'),
('208920100001103', 'User6', 0, 0, 10000000, 0, 1, 1, 0, '6'),
('208920100001104', 'User7', 0, 0, 10000000, 0, 1, 1, 0, '7'),
('208920100001105', 'User8', 0, 0, 10000000, 0, 1, 1, 0, '8'),
('208920100001106', 'User9', 0, 0, 10000000, 0, 1, 1, 0, '9'),
('208920100001107', 'User10', 0, 0, 10000000, 0, 1, 1, 0, '10'),
('208920100001108', 'User11', 0, 0, 10000000, 0, 1, 1, 0, '11'),
('208920100001109', 'User12', 0, 0, 10000000, 0, 1, 1, 0, '12'),
('208920100001111', 'User13', 0, 0, 10000000, 0, 1, 1, 0, '13')
;
