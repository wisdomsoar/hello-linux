

几种linux内核文件的区别(vmlinux、zImage、bzImage、uImage、vmlinuz、initrd )
ref: https://blog.csdn.net/hanxuefan/article/details/7454352#:~:targetText=1%E3%80%81vmlinux%20%E7%BC%96%E8%AF%91%E5%87%BA%E6%9D%A5%E7%9A%84,%E9%AB%98%E7%AB%AF%E5%86%85%E5%AD%98(1M%E4%BB%A5%E4%B8%8A)%E3%80%82


zImage是ARM Linux常用的一种压缩映像文件，
uImage是U-boot专用的映像文件，它是在zImage之前加上一个长度为0x40的“头”，说明这个映像文件的类型、加载位置、生成时间、大小等信息。换句话说，如果直接从uImage的0x40位置开始执行，zImage和uImage没有任何区别

1、vmlinux  编译出来的最原始的内核文件，未压缩。
   vmlinuz是可引导的、压缩的内核。“vm”代表“Virtual Memory”。Linux 支持虚拟内存
   Linux能够使用硬盘空间作为虚拟内存，因此得名“vm”
   
2、zImage   是vmlinux经过gzip压缩后的文件。
   make zImage
   
   copy imx6-4.1.15_2.0.0/build-imx-qsip2/tmp/deploy/images/imx6dlsabresd_qsip2 path/to/local

3、bzImage bz表示“big zImage”，不是用bzip2压缩的。两者的不同之处在于，zImage解压缩内核到低端内存(第一个640K)，bzImage解压缩内核到高端内存(1M以上)。如果内核比较小，那么采用zImage或bzImage都行，如果比较大应该用bzImage。

4、uImage   U-boot专用的映像文件，它是在zImage之前加上一个长度为0x40的tag。

5、vmlinuz  是bzImage/zImage文件的拷贝或指向bzImage/zImage的链接。

6、initrd   是“initial ramdisk”的简写。一般被用来临时的引导硬件到实际内核vmlinuz能够接管并继续引导的状态。
