CREATE TABLE `account` (
  `platform_id` int(10) NOT NULL,
  `server_id` int(10) NOT NULL,
  `user_id` char(50) NOT NULL,
  `account_id` bigint(20) NOT NULL,
  `role_id` bigint(20) NOT NULL,
  PRIMARY KEY (`role_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE `role` (
  `account_id` bigint(20) NOT NULL,
  `role_id` bigint(20) NOT NULL,
  `sex` tinyint(3) NOT NULL,
  `level` tinyint(3) NOT NULL,
  `role_name` char(50) COLLATE utf8_bin NOT NULL,
  `buildings` varchar(10000) COLLATE utf8_bin NOT NULL,
  `resources` varchar(1000) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`role_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE `sequence` (
  `sequence_type` tinyint(3) NOT NULL,
  `process_id` smallint(4) NOT NULL,
  `sequence_id` int(10) NOT NULL,
  PRIMARY KEY (`sequence_type`,`process_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;