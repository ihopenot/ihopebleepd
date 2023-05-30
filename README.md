# Hanshow CC2640 BLE E-Paper

汉朔电子价签的固件和蓝牙操作的web界面。

固件适配TI CC2640主控，实测能用的汉朔电子价签型号为Stellar-M3N@ E31A和Stellar-L3N@ E31A。

固件是基于https://github.com/muyuchl/cc2640BleEpd这个仓库的源码改的。添加了4阶灰度的实现和对接了下web界面，原作者的android app有点难用。

web界面是基于https://github.com/reece15/stellar-L3N-etag这个仓库的web_tools改的，删了点固件里没实现的功能，加了些灰度图相关的功能。因为传图的时候有时候蓝牙会自己断掉，不知道什么原因，所以在web端还加上了断线重发的功能。

## Install

源码编译参考https://github.com/muyuchl/cc2640BleEpd/blob/main/doc/usage.txt和https://www.cnblogs.com/myfish3/p/14404414.html

编译时添加宏EPD_4IN2指定4.2寸屏版本，否则默认编译2.13寸的屏

刷固件用的xds110走jtag刷入

## Result

<img src="img/result1.jpg" alt="result1" style="zoom:50%;" />

