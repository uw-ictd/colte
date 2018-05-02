
DROP TABLE IF EXISTS `customers`;
CREATE TABLE `customers` (
  `idcustomers` int(11) NOT NULL AUTO_INCREMENT,
  `imsi` varchar(16) NOT NULL,
  `raw_down` int(10) unsigned DEFAULT '0',
  `raw_up` int(10) unsigned DEFAULT '0',
  `balance` int(10) unsigned DEFAULT '0' COMMENT 'in USD for now',
  `enabled` tinyint(1) DEFAULT '0',
  `msisdn` varchar(16) DEFAULT 'NotUsed',
  PRIMARY KEY (`idcustomers`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `customers` VALUES (1, '910540000000999', 0, 0, 500, 1, 'NotUsed');
