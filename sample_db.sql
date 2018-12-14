-- MySQL dump 10.13  Distrib 5.7.18, for Linux (x86_64)
--
-- Host: localhost    Database: colte_db
-- ------------------------------------------------------
-- Server version	5.7.18-0ubuntu0.17.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `apn`
--

DROP TABLE IF EXISTS `apn`;
DROP TABLE IF EXISTS `pgw`;
DROP TABLE IF EXISTS `terminal-info`;

--
-- Table structure for table `mmeidentity`
--

DROP TABLE IF EXISTS `mmeidentity`;
CREATE TABLE `mmeidentity` (
  `idmmeidentity` int(11) NOT NULL AUTO_INCREMENT,
  `mmehost` varchar(255) DEFAULT NULL,
  `mmerealm` varchar(200) DEFAULT NULL,
  `UE-Reachability` tinyint(1) NOT NULL COMMENT 'Indicates whether the MME supports UE Reachability Notifcation',
  PRIMARY KEY (`idmmeidentity`)
) ENGINE=MyISAM AUTO_INCREMENT=46 DEFAULT CHARSET=latin1;

INSERT INTO `mmeidentity` VALUES (1,'mme.OpenAir5G.Alliance','OpenAir5G.Alliance',0);

--
-- Table structure for table `pdn`
--

DROP TABLE IF EXISTS `pdn`;
CREATE TABLE `pdn` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `apn` varchar(60) NOT NULL DEFAULT 'ltebox',
  `pdn_type` enum('IPv4','IPv6','IPv4v6','IPv4_or_IPv6') NOT NULL DEFAULT 'IPv4',
  `pdn_ipv4` varchar(15) DEFAULT '0.0.0.0',
  `pdn_ipv6` varchar(45) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT '0:0:0:0:0:0:0:0',
  `aggregate_ambr_ul` int(10) unsigned DEFAULT '50000000',
  `aggregate_ambr_dl` int(10) unsigned DEFAULT '100000000',
  `pgw_id` int(11) NOT NULL DEFAULT 3,
  `users_imsi` varchar(15) NOT NULL,
  `qci` tinyint(3) unsigned NOT NULL DEFAULT '9',
  `priority_level` tinyint(3) unsigned NOT NULL DEFAULT '15',
  `pre_emp_cap` enum('ENABLED','DISABLED') DEFAULT 'DISABLED',
  `pre_emp_vul` enum('ENABLED','DISABLED') DEFAULT 'ENABLED',
  `LIPA-Permissions` enum('LIPA-prohibited','LIPA-only','LIPA-conditional') NOT NULL DEFAULT 'LIPA-only',
  PRIMARY KEY (`id`,`pgw_id`,`users_imsi`),
  KEY `fk_pdn_pgw1_idx` (`pgw_id`),
  KEY `fk_pdn_users1_idx` (`users_imsi`)
) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=latin1;

INSERT INTO `pdn` VALUES 
(43,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'001010123456789',9,15,'DISABLED','ENABLED','LIPA-only'),
(22,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001100',9,15,'DISABLED','ENABLED','LIPA-only'),
(23,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001101',9,15,'DISABLED','ENABLED','LIPA-only'),
(24,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001102',9,15,'DISABLED','ENABLED','LIPA-only'),
(25,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001103',9,15,'DISABLED','ENABLED','LIPA-only'),
(26,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001104',9,15,'DISABLED','ENABLED','LIPA-only'),
(27,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001105',9,15,'DISABLED','ENABLED','LIPA-only'),
(28,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001106',9,15,'DISABLED','ENABLED','LIPA-only'),
(29,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001107',9,15,'DISABLED','ENABLED','LIPA-only'),
(30,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001108',9,15,'DISABLED','ENABLED','LIPA-only'),
(31,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001109',9,15,'DISABLED','ENABLED','LIPA-only'),
(60,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'208920100001111',9,15,'DISABLED','ENABLED','LIPA-only');

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `imsi` varchar(15) NOT NULL COMMENT 'IMSI is the main reference key.',
  `msisdn` varchar(46) DEFAULT NULL COMMENT 'The basic MSISDN of the UE (Presence of MSISDN is optional).',
  `imei` varchar(15) DEFAULT NULL COMMENT 'International Mobile Equipment Identity',
  `imei_sv` varchar(2) DEFAULT NULL COMMENT 'International Mobile Equipment Identity Software Version Number',
  `ms_ps_status` enum('PURGED','NOT_PURGED') DEFAULT 'PURGED' COMMENT 'Indicates that ESM and EMM status are purged from MME',
  `rau_tau_timer` int(10) unsigned DEFAULT '120',
  `ue_ambr_ul` bigint(20) unsigned DEFAULT '50000000' COMMENT 'The Maximum Aggregated uplink MBRs to be shared across all Non-GBR bearers according to the subscription of the user.',
  `ue_ambr_dl` bigint(20) unsigned DEFAULT '100000000' COMMENT 'The Maximum Aggregated downlink MBRs to be shared across all Non-GBR bearers according to the subscription of the user.',
  `access_restriction` int(10) unsigned DEFAULT '47' COMMENT 'Indicates the access restriction subscription information. 3GPP TS.29272 #7.3.31',
  `mme_cap` int(10) unsigned zerofill DEFAULT 0 COMMENT 'Indicates the capabilities of the MME with respect to core functionality e.g. regional access restrictions.',
  `mmeidentity_idmmeidentity` int(11) NOT NULL DEFAULT '1',
  `key` varbinary(16) NOT NULL DEFAULT '0' COMMENT 'UE security key',
  `RFSP-Index` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'An index to specific RRM configuration in the E-UTRAN. Possible values from 1 to 256',
  `urrp_mme` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'UE Reachability Request Parameter indicating that UE activity notification from MME has been requested by the HSS.',
  `sqn` bigint(20) unsigned zerofill NOT NULL DEFAULT '351',
 `rand` varbinary(16) NOT NULL DEFAULT '0x0',
  `OPc` varbinary(16) DEFAULT NULL DEFAULT '0x0' COMMENT 'Can be computed by HSS',
  PRIMARY KEY (`imsi`,`mmeidentity_idmmeidentity`),
  KEY `fk_users_mmeidentity_idx1` (`mmeidentity_idmmeidentity`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `users` VALUES 
('001010123456789','33600101789','35609204079298',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001100','33638020000','35609204079200',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001101','33638020001','35609204079201',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001102','33638020002','35609204079202',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001103','33638020003','35609204079203',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001104','33638020004','35609204079204',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001105','33638020005','35609204079205',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001106','33638020006','35609204079206',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001107','33638020007','35609204079207',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001108','33638020008','35609204079208',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001109','33638020009','35609204079209',NULL,'PURGED',120,50000000,100000000,47,0,1,0x6874736969202073796d4b2079650a73,1,0,351,0x0,0x504f20634f6320504f50206363500a4f),
('208920100001111','33611123456','356113022094149',NULL,'PURGED',120,50000000,100000000,47,0,1,0x8baf473f2f8fd09487cccbd7097c6862,1,0,351,0xd8e9675fb0c50c56a1ce914c3da88a1c,0xe734f8734007d6c5ce7a0508809e7e9c)
;

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
  `raw_down` int(10) unsigned DEFAULT '0',
  `raw_up` int(10) unsigned DEFAULT '0',
  `data_balance` bigint(15) DEFAULT '10000000',
  `balance` decimal(13,4) DEFAULT '0' COMMENT 'this value is currency-less',
  `bridged` tinyint(1) DEFAULT '1',
  `enabled` tinyint(1) DEFAULT '1',
  `admin` tinyint(1) DEFAULT '0',
  `msisdn` varchar(16) NOT NULL,
  PRIMARY KEY (`imsi`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `customers` VALUES
('000000000000001', 0, 0, 10000000, 0, 1, 1, 1, '1'),
('001010123456789', 0, 0, 10000000, 0, 1, 1, 0, '2'),
('208920100001100', 0, 0, 10000000, 0, 1, 1, 0, '3'),
('208920100001101', 0, 0, 10000000, 0, 1, 1, 0, '4'),
('208920100001102', 0, 0, 10000000, 0, 1, 1, 0, '5'),
('208920100001103', 0, 0, 10000000, 0, 1, 1, 0, '6'),
('208920100001104', 0, 0, 10000000, 0, 1, 1, 0, '7'),
('208920100001105', 0, 0, 10000000, 0, 1, 1, 0, '8'),
('208920100001106', 0, 0, 10000000, 0, 1, 1, 0, '9'),
('208920100001107', 0, 0, 10000000, 0, 1, 1, 0, '10'),
('208920100001108', 0, 0, 10000000, 0, 1, 1, 0, '11'),
('208920100001109', 0, 0, 10000000, 0, 1, 1, 0, '12'),
('208920100001111', 0, 0, 10000000, 0, 1, 1, 0, '13')
;
