
DROP TABLE IF EXISTS `customers`;
CREATE TABLE `customers` (
  `idcustomers` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(16) NOT NULL,
  `raw_down` int(10) unsigned DEFAULT '0',
  `raw_up` int(10) unsigned DEFAULT '0',
  `balance` int(10) unsigned DEFAULT '0' COMMENT 'in USD for now',
  `imsi` varchar(16) DEFAULT 'NotUsed',
  `msisdn` varchar(16) DEFAULT 'NotUsed',
  PRIMARY KEY (`idcustomers`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `customers` VALUES (1, '10.0.0.42', 0, 0, 500, 'NotUsed', 'NotUsed');
