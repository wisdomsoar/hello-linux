
https://www.cntofu.com/book/46/linux_tools/README.md

https://elixir.bootlin.com/linux/latest/source/drivers

幾種linux內核檔的區別(vmlinux、zImage、bzImage、uImage、vmlinuz、initrd )

ref: https://blog.csdn.net/hanxuefan/article/details/7454352#:~:targetText=1%E3%80%81vmlinux%20%E7%BC%96%E8%AF%91%E5%87%BA%E6%9D%A5%E7%9A%84,%E9%AB%98%E7%AB%AF%E5%86%85%E5%AD%98(1M%E4%BB%A5%E4%B8%8A)%E3%80%82


zImage是ARM Linux常用的一種壓縮映像檔，
uImage是U-boot專用的映像檔，它是在zImage之前加上一個長度為0x40的“頭”，說明這個映像檔的類型、載入位置、生成時間、大小等資訊。換句話說，如果直接從uImage的0x40位置開始執行，zImage和uImage沒有任何區別

1、vmlinux  編譯出來的最原始的內核檔，未壓縮。
   vmlinuz是可引導的、壓縮的內核。“vm”代表“Virtual Memory”。Linux 支持虛擬記憶體
   Linux能夠使用硬碟空間作為虛擬記憶體，因此得名“vm”
   
2、zImage   是vmlinux經過gzip壓縮後的檔。
   make zImage
   
   copy imx6-4.1.15_2.0.0/build-imx-qsip2/tmp/deploy/images/imx6dlsabresd_qsip2 path/to/local

3、bzImage bz表示“big zImage”，不是用bzip2壓縮的。兩者的不同之處在于，zImage解壓縮內核到低端記憶體(第一個640K)，bzImage解壓縮內核到高端記憶體(1M以上)。如果內核比較小，那麼採用zImage或bzImage都行，如果比較大應該用bzImage。

4、uImage   U-boot專用的映射檔，它是在zImage之前加上一個長度為0x40的tag。

5、vmlinuz  是bzImage/zImage檔的拷貝或指向bzImage/zImage的連結。

6、initrd   是“initial ramdisk”的簡寫。一般被用來臨時的引導硬體到實際內核vmlinuz能夠接管並繼續引導的狀態。
