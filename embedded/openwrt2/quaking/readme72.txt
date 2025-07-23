
git clone git@ip:sdx72_sdk/xxxxxxxrepo.git

zyrepo/zyrepo init -u git@ip:sdx72_sdk/manifest.git -m xxxxxxxxxxxx2-2.xml -b main

xxxxxxxrepo/xxxxxxxrepo sync

cd prpl/

git checkout prpl-sah-next

cd ../BSP/p???????_apps/apps_proc/owrt/

source ./configure ??????_Generic

build_kernel sdx75 perf

make P=???????_Generic -j8


system.img在哪
BSP/p??????????????_apps/apps_proc/owrt/bin/targets/perf/sdx75/cpe-ab
link:
Z:\x72_owrt\bin\targets\perf\sdx75\cpe-ab


看一下webui在哪
 pwd
/?????/prpl/feed_prpl/network/utilities/prpl-webui

vi Makefile

PKG_NAME:=prpl-webui
PKG_RELEASE:=1
PKG_LICENSE:=BSD-2-Clause-Patent

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git@????????????????????:sdx72_sdk/webgui-vue3.git