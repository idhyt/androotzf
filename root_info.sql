/*
 Navicat Premium Data Transfer

 Source Server         : androotf
 Source Server Type    : SQLite
 Source Server Version : 3030001
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3030001
 File Encoding         : 65001

 Date: 09/07/2017 18:13:12
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for root_info
-- ----------------------------
DROP TABLE IF EXISTS "root_info";
CREATE TABLE "root_info" (
	id INTEGER NOT NULL, 
	root_id INTEGER NOT NULL, 
	product_model VARCHAR(32) NOT NULL, 
	proc_version VARCHAR(512) NOT NULL, 
	root_parameters VARCHAR(255) NOT NULL, 
	date TIMESTAMP, priority INTEGER, build_version  VARCHAR(32), 
	PRIMARY KEY (id)
);

-- ----------------------------
-- Records of root_info
-- ----------------------------
BEGIN;
INSERT INTO "root_info" VALUES (1, 1021, 'JAZZ', 'Linux version 3.10.86-g76ac748 (android@localhost) (gcc version 4.7.3 20121205 (prerelease) (crosstool-NG linaro-1.13.1-4.7-2012.12-20121214 - Linaro GCC 2012.12) ) #2 SMP PREEMPT Mon Apr 4 05:03:33 CST 2016', 'n=1021&k=[101=0xc15b0454;102=0x20;104=0xC139AAE0;105=0x68;106=0xC09BF2D0;109=0x10;801=0xc13b554c;802=0xc15a955c;]&j=[0x9c=0xaaaaaaaa;0xc=0xC0DDB6B0;0x18=0xaaaaaaaa;0x24=0xC0702CE0;0x0=0x0;]&p=[0xc15a7990=0x0=0x4;0xc15b0474=0xc091eb2c=0x4;]', NULL, 10, NULL);
INSERT INTO "root_info" VALUES (2, 1021, 'Nexus 5', 'Linux version 3.4.0-gbebb36b (android-build@vpbs1.mtv.corp.google.com) (gcc version 4.7 (GCC) ) #1 SMP PREEMPT Tue Mar 10 18:17:45 UTC 2015', 'n=1021&k=[101=0xc1237658;102=0x20;104=0xc1042f38;105=0x58;106=0xc05cfc70;109=0x4;]&j=[0xf8=0xaaaaaaaa;0xc=0xC085ED40;0x18=0xaaaaaaaa;0x24=0xc027ac48;0x0=0x0;]&p=[0xc122dae4=0x0=0x4;0xc1237678=0xC047247C=0x4;]', NULL, 10, '5.1.1');
INSERT INTO "root_info" VALUES (3, 1021, 'C6903', 'Linux version 3.4.0-perf-g2deedd1 (BuildUser@BuildHost) (gcc version 4.8 (GCC) ) #1 SMP PREEMPT Thu Aug 20 14:52:56 2015', 'n=1021&k=[101=0xc10ecb90;102=0x20;104=0xc0f1f700;105=0x58;106=0xC0500D7C;109=0x4;]&j=[0xf8=0xaaaaaaaa;0xc=0xC0787914;0x18=0xaaaaaaaa;0x24=0xC026E5A0;0x0=0x0;]&p=[0xc10dfc98=0x0=0x4;0xc10ecbb0=0xc04185b4=0x4;]', NULL, 10, '5.1.1');
INSERT INTO "root_info" VALUES (4, 1001, 'Nexus 6P', 'Linux version 3.10.73-g5b0be8f02fe (android-build@wphs1.hot.corp.google.com) (gcc version 4.9.x-google 20140827 (prerelease) (GCC) ) #1 SMP PREEMPT Thu Jun 8 18:03:16 UTC 2017', 'n=1001&k=[101=0xffffffc001a044a0;102=0x48;104=0xffffffc001779fe0;105=0x70;201=0xffffffc00074c954;]&j=[0x180=0xaaaaaaaa;0x158=0xbbbbbbbb;0x2d0=0xffffffc00024c2c4;0x0=0x0;0x00=0xffffffc000afe07c;0x28=0xbbbbbbbb;0x48=0xffffffc0002ef958;0x90=0xdddddddd;0x10=0xffffffc000ce6000;0x8=0xffffffc000318610;0x0=0x0;]&p=[0xffffffc00193a1bc=0x0=0x4;0xffffffc001a044e8=0xffffffc00055bab8=0x8;]', NULL, 10, '7.1.2');
INSERT INTO "root_info" VALUES (5, 999, 'testcase', 'testcase', 'testcase', NULL, 10, '0.0.0');
INSERT INTO "root_info" VALUES (6, 1002, 'generic_a15', 'Linux version 4.1.18-gfd75bbb (android@localhost) (gcc version 4.9 20150123 (prerelease) (GCC) ) #1 SMP PREEMPT Thu Mar 2 17:29:20 CST 2017', 'n=1002&k=[101=0xffffffc001de7ad0;102=0x40;104=0xffffffc00198efc0;105=0x78;201=0xffffffc0000f7f2c;]&j=[0x60000003e=0x600000326;0x600000376=0xffffffc000f54e14;0x600000136=0xffffffc0005b5844;0x60000013e=0xffffffc001de7b08;0x6000000d6=0x600000326;0x6000000de=0xffffffc000f490f0;0x600000346=0xffffffc00060d0d0;0x6000004de=0x600000226;0x60000028e=0xffffffc00010314c;0x0=0x0;0xc100020=0x0c100100;0x0c100100=0xffffffc000f2eff4;0xc100108=0x0c100200;0xc100228=0x0c100300;0xc100348=0xffffffc0007d5788;0xc1002b0=0xffffffc001980000;0xc100378=0x0c100400;0xc100428=0xffffffc0001e91a4;0x0=0x0;]&p=[0xffffffc001d99b14=0x0=0x4;]', NULL, 10, 7.0);
INSERT INTO "root_info" VALUES (7, 1002, 'generic_a15', 'Linux version 4.1.18-gf5b4220 (android@localhost) (gcc version 4.9 20150123 (prerelease) (GCC) ) #1 SMP PREEMPT Wed Apr 12 03:17:23 CST 2017', 'n=1003&k=[101=0xffffffc001dfbaf8;102=0x40;104=0xffffffc001996fc0;105=0x78;109=0x8;201=0xffffffc0000f8348;]&j=[0x60000003e=0x600000326;0x600000376=0xffffffc000f58fb0;0x600000136=0xffffffc0005b8210;0x60000013e=0xffffffc001dfbb30;0x6000000d6=0x600000326;0x6000000de=0xffffffc000f4d28c;0x600000346=0xffffffc00060e848;0x6000004de=0x600000226;0x60000028e=0xffffffc00010356c;0x0=0x0;0xc100020=0x0c100100;0x0c100100=0xffffffc000f331a4;0xc100108=0x0c100200;0xc100228=0x0c100300;0xc100348=0xffffffc0007d7fb0;0xc1002b0=0xffffffc001988000;0xc100378=0x0c100400;0xc100428=0xffffffc0001e97ac;0x0=0x0;]&p=[0xffffffc001dabb54=0x0=0x4;0xffffffc001dfbb38=0xffffffc00054b744=0x8;]', NULL, 10, 7.0);
INSERT INTO "root_info" VALUES (8, 1021, 'SM-A700L', 'Linux version 3.10.49-6845695 (dpi@SWDD6719) (gcc version 4.8 (GCC) ) #1 SMP PREEMPT Thu Feb 18 16:30:46 KST 2016', 'n=1021&k=[101=0xc1378310;102=0x20;104=0xc1125f40;105=0x5c;106=0xc0542330;109=0x4;&j=[0x10c=0xaaaaaaaa;0xc=0xc08bd9bc;0x18=0xaaaaaaaa;0x24=0xC0202D70;0x0=0x0;]&p=[0xc12c38e0=0x0=0x4;0xc1378330=0xc04338f0=0x4;]', NULL, 10, '5.0.2');
INSERT INTO "root_info" VALUES (9, 1003, 'generic_a15', 'Linux version 4.1.18-gf5b4220 (android@localhost) (gcc version 4.9 20150123 (prerelease) (GCC) ) #1 SMP PREEMPT Wed Apr 12 03:17:23 CST 2017', 'n=1003&k=[200=0xffffffc0000f8348;101=0xffffffc001dfbaf8;103=0xa8;104=0xffffffc001996fc0;105=0x78;109=0x4;]&j=[0x18=0xaaaaaaaa;0x50=0xffffffc0005b8210;0xb0=0xaaaaaaaa;0xb8=0xffffffc000f331a4;0x20=0xffffffc000f58fb0;0x110=0xffffffc00060e848;0x118=0xbbbbbbbb;0x4b8=0xaaaaaaaa;0x68=0xFFFFFFC00010356C;0x0=0x0;0x28=0xaaaaaaaa;0x48=0xffffffc0001e84e4;0x0=0x0;]&p=[0xffffffc001dabb54=0x0=0x4;0xffffffc001dfbba0=0x0=0x8;]', NULL, 9, 7.0);
INSERT INTO "root_info" VALUES (10, 1003, 'hi6250', 'Linux version 3.10.90-g1e8f015 (android@localhost) (gcc version 4.9.x-google 20140827 (prerelease) (GCC) ) #1 SMP PREEMPT Fri Jan 20 03:30:14 CST 2017', 'n=1003&k=[200=0xffffffc00020a6dc;101=0xffffffc001674b10;103=0xa0;104=0xffffffc00135c860;105=0x70;109=0x4;801=0xffffffc00166aa10;802=0xffffffc00166aa28;]&j=[0x20=0xcccccccc;0x100=0xffffffc000205528;0x108=0xdddddddd;0x118=0xdddddddd;0x110=0xeeeeeeee;0x218=0xdddddddd;0x290=0xdddddddd;0x210=0xffffffc000348510;0x238=0xffffffff;0x310=0xffffffc000121308;0x328=0xffffffc000ab07c4;0x0=0x0;0x28=0xaaaaaaaa;0x48=0xffffffc0001aacd8;0x0=0x0;]&p=[0xffffffc0016672c4=0x0=0x4;0xffffffc001674bb0=0x0=0x8;]', NULL, 10, 6.0);
INSERT INTO "root_info" VALUES (11, 1022, 'Nexus 5', 'Linux version 3.4.0-gbebb36b (android-build@vpbs1.mtv.corp.google.com) (gcc version 4.7 (GCC) ) #1 SMP PREEMPT Tue Mar 10 18:17:45 UTC 2015', 'n=1022&k=[101=0xc1237658;103=0x50;104=0xc1042f38;105=0x58;107=0xc085ed40;109=0x4;]&j=[0x18=0xaaaaaaaa;0x24=0xc0279bd0;0x0=0x0;]&p=[0xc122dae4=0x0=0x4;0xc12376a8=0x0=0x4;]', NULL, 9, '6.0.1');
INSERT INTO "root_info" VALUES (12, 1022, 'JAZZ', 'Linux version 3.10.86-g76ac748 (android@localhost) (gcc version 4.7.3 20121205 (prerelease) (crosstool-NG linaro-1.13.1-4.7-2012.12-20121214 - Linaro GCC 2012.12) ) #2 SMP PREEMPT Mon Apr 4 05:03:33 CST 2016', 'n=1022&k=[101=0xc15b0454;103=0x50;104=0xc139aae0;105=0x68;107=0xc0ddb6b0;109=0x10;801=0xc13b554c;802=0xc15a955c;]&j=[0x18=0xaaaaaaaa;0x24=0xc0701c78;0x0=0x0;]&p=[0xc15a7990=0x0=0x4;0xc15b04a4=0x0=0x4;]', NULL, 9, 6.0);
INSERT INTO "root_info" VALUES (13, 1022, 'Nexus 5', 'Linux version 3.4.0-g2aa165e (android-build@wped19.hot.corp.google.com) (gcc version 4.8 (GCC) ) #1 SMP PREEMPT Thu Aug 20 06:07:34 UTC 2015', 'n=1022&k=[101=0xc1338250;103=0x50;104=0xc1142f80;105=0x58;107=0xc088b1c4;109=0x4;]&j=[0x18=0xaaaaaaaa;0x24=0xc0281d04;0x0=0x0;]&p=[0xc132e6a8=0x0=0x4;0xc13382a0=0x0=0x4;]', NULL, 10, '5.1.1');
INSERT INTO "root_info" VALUES (14, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.4.4');
INSERT INTO "root_info" VALUES (15, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.4.3');
INSERT INTO "root_info" VALUES (16, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.4.2');
INSERT INTO "root_info" VALUES (17, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.4.1');
INSERT INTO "root_info" VALUES (18, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 4.4);
INSERT INTO "root_info" VALUES (19, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.3.1');
INSERT INTO "root_info" VALUES (20, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 4.3);
INSERT INTO "root_info" VALUES (21, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.2.2');
INSERT INTO "root_info" VALUES (22, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 4.2);
INSERT INTO "root_info" VALUES (23, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.1.2');
INSERT INTO "root_info" VALUES (24, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.1.1');
INSERT INTO "root_info" VALUES (25, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 4.1);
INSERT INTO "root_info" VALUES (26, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.0.4');
INSERT INTO "root_info" VALUES (27, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.0.3');
INSERT INTO "root_info" VALUES (28, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.0.2');
INSERT INTO "root_info" VALUES (29, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '4.0.1');
INSERT INTO "root_info" VALUES (30, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 4.0);
INSERT INTO "root_info" VALUES (31, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '3.2.6');
INSERT INTO "root_info" VALUES (32, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '3.2.5');
INSERT INTO "root_info" VALUES (33, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '3.2.4');
INSERT INTO "root_info" VALUES (34, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '3.2.3');
INSERT INTO "root_info" VALUES (35, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '3.2.2');
INSERT INTO "root_info" VALUES (36, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '3.2.1');
INSERT INTO "root_info" VALUES (37, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 3.2);
INSERT INTO "root_info" VALUES (38, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 3.1);
INSERT INTO "root_info" VALUES (39, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.3.6');
INSERT INTO "root_info" VALUES (40, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.3.5');
INSERT INTO "root_info" VALUES (41, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.3.4');
INSERT INTO "root_info" VALUES (42, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.3.3');
INSERT INTO "root_info" VALUES (43, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.3.2');
INSERT INTO "root_info" VALUES (44, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.3.1');
INSERT INTO "root_info" VALUES (45, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 2.3);
INSERT INTO "root_info" VALUES (46, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.2.3');
INSERT INTO "root_info" VALUES (47, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.2.2');
INSERT INTO "root_info" VALUES (48, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.2.1');
INSERT INTO "root_info" VALUES (49, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 2.2);
INSERT INTO "root_info" VALUES (50, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 2.1);
INSERT INTO "root_info" VALUES (51, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, '2.0.1');
INSERT INTO "root_info" VALUES (52, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 2.0);
INSERT INTO "root_info" VALUES (53, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 1.6);
INSERT INTO "root_info" VALUES (54, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 1.5);
INSERT INTO "root_info" VALUES (55, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 1.1);
INSERT INTO "root_info" VALUES (56, 1051, 'NULL', 'NULL', 'n=1051&k=[]&j=[]&p=[]', NULL, 10, 1.0);
INSERT INTO "root_info" VALUES (57, 1004, 'SM-A800S', 'Linux version 3.10.9-9468772 (dpi@SWDD5623) (gcc version 4.8 (GCC) ) #1 SMP PREEMPT Thu Oct 20 17:39:00 KST 2016', 'n=1004&k=[101=0xc111dd78;103=0x50;104=0xc0dece80;105=0x5c;109=0x4;811=0x1;]&p=[0xc11184dc=0x0=0x4;0xc111ddc8=0x0=0x4;]', NULL, 10, '6.0.1');
INSERT INTO "root_info" VALUES (58, 1004, 'Nexus 5', 'Linux version 3.4.0-gcf10b7e (android-build@wpiv11.hot.corp.google.com) (gcc version 4.8 (GCC) ) #1 SMP PREEMPT Mon Sep 19 22:14:08 UTC 2016', 'n=1004&k=[101=0xc13380c8;103=0x50;104=0xc1142f80;105=0x58;109=0x4;]&p=[0xc132e460=0x0=0x4;0xc1338118=0x0=0x4;]', NULL, 10, '6.0.1');
INSERT INTO "root_info" VALUES (59, 1022, 'SM-G900L', 'Linux version 3.4.0-6309255 (dpi@SWDD6912) (gcc version 4.8 (GCC) ) #1 SMP PREEMPT Fri Dec 4 15:28:30 KST 2015', 'n=1022&k=[101=0xc14444b8;103=0x50;104=0xc1120f20;105=0x58;107=0xc085167c;109=0x4;811=0x1;]&j=[0x18=0xaaaaaaaa;0x24=0xc02697c0;0x0=0x0;]&p=[0xc142da78=0x0=0x4;0xc1444508=0x0=0x4;]', NULL, 10, 5.0);
INSERT INTO "root_info" VALUES (60, 1003, 'Nexus 6P', 'Linux version 3.10.73-g5b0be8f02fe (android-build@wphs1.hot.corp.google.com) (gcc version 4.9.x-google 20140827 (prerelease) (GCC) ) #1 SMP PREEMPT Thu Jun 8 18:03:16 UTC 2017', 'n=1003&k=[101=0xffffffc001a044a0;103=0xa8;104=0xffffffc001779fe0;105=0x70;109=0x4;200=0xffffffc00074c954;]&j=[0x180=0xaaaaaaaa;0x158=0xaaaabbbb;0x2d0=0xffffffc000afe07c;0x0=0x0;0x28=0xaaaaaaaa;0x48=0xffffffc0003176c4;0x0=0x0;]&p=[0xffffffc00193a1bc=0x0=0x4;0xffffffc001a04548=0x0=0x8;]', NULL, 11, '7.1.2');
INSERT INTO "root_info" VALUES (61, 1003, 'generic_a15', 'Linux version 4.1.18-gf6402d0 (android@localhost) (gcc version 4.9 20150123 (prerelease) (GCC) ) #1 SMP PREEMPT Tue Jan 10 12:54:47 CST 2017', 'n=1003&k=[200=0xffffffc0000f0d88;101=0xffffffc001b8da48;103=0xa8;104=0xffffffc001692e00;105=0x70;109=0x8;]&j=[0x18=0xcccccccc;0x150=0xFFFFFFC000E1B148;0x50=0xFFFFFFC00052F814;0x60=0xbbbbbbbb;0xb0=0xcccccccc;0xb8=0xffffffc000cfcc74;0x120=0xFFFFFFC000562DC4;0x4b0=0xaaaaaaaa;0x68=0xffffffc0000fb1ec;0x0=0x0;0x28=0xaaaaaaaa;0x48=0xffffffc0001ce6e4;0x0=0x0;]&p=[0xffffffc001b5df5c=0x0=0x4;]', NULL, 10, 7.0);
COMMIT;

PRAGMA foreign_keys = true;
