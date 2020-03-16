DROP TABLE IF EXISTS `customers`;
DROP TABLE IF EXISTS `static_ips`;
DROP TABLE IF EXISTS `dnsResponses`;
DROP TABLE IF EXISTS `answers`;
DROP TABLE IF EXISTS `flowlogs`;

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

CREATE TABLE `static_ips` (
  `imsi` varchar(16) NOT NULL,
  `ip` varchar(16) NOT NULL,
  PRIMARY KEY (`imsi`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
