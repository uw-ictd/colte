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
  `apn` varchar(60) NOT NULL,
  `pdn_type` enum('IPv4','IPv6','IPv4v6','IPv4_or_IPv6') NOT NULL DEFAULT 'IPv4',
  `pdn_ipv4` varchar(15) DEFAULT '0.0.0.0',
  `pdn_ipv6` varchar(45) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT '0:0:0:0:0:0:0:0',
  `aggregate_ambr_ul` int(10) unsigned DEFAULT '50000000',
  `aggregate_ambr_dl` int(10) unsigned DEFAULT '100000000',
  `pgw_id` int(11) NOT NULL,
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
(60,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'910540000000995',9,15,'DISABLED','ENABLED','LIPA-only'),
(61,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'910540000000996',9,15,'DISABLED','ENABLED','LIPA-only'),
(62,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'910540000000997',9,15,'DISABLED','ENABLED','LIPA-only'),
(63,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'910540000000998',9,15,'DISABLED','ENABLED','LIPA-only'),
(64,'ltebox','IPv4','0.0.0.0','0:0:0:0:0:0:0:0',50000000,100000000,3,'910540000000999',9,15,'DISABLED','ENABLED','LIPA-only')
;

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
  `mme_cap` int(10) unsigned zerofill DEFAULT NULL COMMENT 'Indicates the capabilities of the MME with respect to core functionality e.g. regional access restrictions.',
  `mmeidentity_idmmeidentity` int(11) NOT NULL DEFAULT '1',
  `key` varbinary(16) NOT NULL DEFAULT '0' COMMENT 'UE security key',
  `RFSP-Index` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'An index to specific RRM configuration in the E-UTRAN. Possible values from 1 to 256',
  `urrp_mme` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'UE Reachability Request Parameter indicating that UE activity notification from MME has been requested by the HSS.',
  `sqn` bigint(20) unsigned zerofill NOT NULL,
 `rand` varbinary(16) NOT NULL,
  `OPc` varbinary(16) DEFAULT NULL COMMENT 'Can be computed by HSS',
  PRIMARY KEY (`imsi`,`mmeidentity_idmmeidentity`),
  KEY `fk_users_mmeidentity_idx1` (`mmeidentity_idmmeidentity`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `users` VALUES
('910540000000995','628650000995',NULL,NULL,'PURGED',120,50000000,100000000,47,0,1,0x85def29fe66f28fa7081857ab98eb77e,1,0,351,0x0,0x826f0df3f2fc5d969e1676e843e61739),
('910540000000996','628650000996',NULL,NULL,'PURGED',120,50000000,100000000,47,0,1,0x6cfa57b83812a4debca0af546253c009,1,0,351,0x0,0xacca1efd2b9611496713ab1381779f35),
('910540000000997','628650000997',NULL,NULL,'PURGED',120,50000000,100000000,47,0,1,0xdd4f8e34bf666ebb4747404b36a2ca45,1,0,351,0x0,0xb142c2a4b45e16940d1239e531b47dfd),
('910540000000998','628650000998',NULL,NULL,'PURGED',120,50000000,100000000,47,0,1,0x5b19bcd10a34fd15173b5432fa5d66e2,1,0,351,0x0,0x1e71bac1bbacf435634a9e7e290b3260),
('910540000000999','628650000999',NULL,NULL,'PURGED',120,50000000,100000000,47,0,1,0x4a0978fcc07b522e7003e1a3695aab0e,1,0,351,0x0,0x992c513693239284fe779a5a177037a0)
;

DROP TABLE IF EXISTS `static_ips`;
CREATE TABLE `static_ips` (
  `imsi` varchar(16) NOT NULL,
  `ip` varchar(16) NOT NULL,
  PRIMARY KEY (`imsi`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `static_ips` VALUES
('000000000000001', '127.0.0.1'),
('910540000000995', '192.168.151.2'),
('910540000000996', '192.168.151.3'),
('910540000000997', '192.168.151.4'),
('910540000000998', '192.168.151.5'),
('910540000000999', '192.168.151.6')
;

DROP TABLE IF EXISTS `customers`;
CREATE TABLE `customers` (
  `imsi` varchar(16) NOT NULL,
  `raw_down` int(10) unsigned DEFAULT '0',
  `raw_up` int(10) unsigned DEFAULT '0',
  `data_balance` int(10) DEFAULT '0',
  `balance` float(10,2) DEFAULT '0.0',
  `bridged` tinyint(1) DEFAULT '0',
  `enabled` tinyint(1) DEFAULT '0',
  `msisdn` varchar(16) DEFAULT 'NotUsed',
  PRIMARY KEY (`imsi`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `customers` VALUES
('000000000000001', 0, 0, 10000000, 500, 1, 1, '0000001'),
('910540000000995', 0, 0, 10000000, 500, 1, 1, '0000002'),
('910540000000996', 0, 0, 10000000, 500, 1, 1, '0000003'),
('910540000000997', 0, 0, 10000000, 500, 1, 1, '0000004'),
('910540000000998', 0, 0, 10000000, 500, 1, 1, '0000005'),
('910540000000999', 0, 0, 10000000, 500, 1, 1, '0000006')
;
