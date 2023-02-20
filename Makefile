# ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk

.PHONY: clean
clean:
	rm -rf obj libs

.PHONY: build
build: clean
	@echo "\033[33m[+] Build for $(RS) \033[0m"
	ndk-build RS=$(RS)

.PHONY: all
all:
	mkdir -p rootz
	make build RS=CVE-2017-8890/1001 && cp -rf libs/armeabi-v7a/rootz rootz/1001
	make build RS=CVE-2017-8890/1002 && cp -rf libs/armeabi-v7a/rootz rootz/1002
	make build RS=CVE-2017-8890/1003 && cp -rf libs/armeabi-v7a/rootz rootz/1003
	make build RS=CVE-2017-8890/1004 && cp -rf libs/armeabi-v7a/rootz rootz/1004
	make build RS=CVE-2017-9077/1010 && cp -rf libs/armeabi-v7a/rootz rootz/1010
	make build RS=CVE-2015-1805/1021 && cp -rf libs/armeabi-v7a/rootz rootz/1021
	make build RS=CVE-2015-1805/1022 && cp -rf libs/armeabi-v7a/rootz rootz/1022
	make build RS=CVE-2016-5195/1051 && cp -rf libs/armeabi-v7a/rootz rootz/1051
	make build RS=CVE-2015-3636/1041 && cp -rf libs/armeabi-v7a/rootz rootz/1041

install: build
	adb shell rm -rf /data/local/tmp/rootz
	adb push libs/armeabi-v7a/rootz /data/local/tmp

test: install
	adb shell chmod 777 /data/local/tmp/rootz
	adb shell /data/local/tmp/rootz
