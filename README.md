# Detecting ROP (and other) attacks from outside


Shortly, it is a method of detecting in-guest ROP attacks out-of-the-box, using Intel LBR.

--------------------------------------------------------------

Usage:

After make and make install, you (the clould provider) can use the commands in the folder which contains LibVMI-based programs to invoke your own CFI or other detection. Other VMI-IDS approaches (e.g. CAPT and CMonitor) are also supported.

Useful links:

CAPT http://www.cic-chinacommunications.cn/EN/abstract/abstract613.shtml

CMonitor https://rd.springer.com/article/10.1007/s11859-014-1030-4

# Quick start

## 1. install modified Xen provided by us

For now, install Xen 4.6 seems a bit of inappropriate because the version is too old, but we only have it.

Xen installing is a trivial thing but time-consuming, you may want to get some help, you could refer to https://github.com/xulai1001/auto-deploy and https://github.com/xulai1001/auto-deploy/blob/master1/rake/xen.rb.

For detailed method about installing Xen, see https://wiki.xenproject.org/wiki/Compiling_Xen_From_Source.

Issues are welcomed.

After finishing Xen installing, you should get a Linux (Ubuntu is the best I guess...) as guest OS.

## 2. install LibVMI

In our project, we use LibVMI version 0.10.1 to run some demos.

For detailed method about installing LibVMI, see https://github.com/libvmi/libvmi.

## 3. Run it!

### 3.1 Creating a HVM guest

Please also refer to Xen homepage..., https://wiki.xenproject.org/wiki/Xen_Project_Beginners_Guide.

### 3.2 using LibVMI

cd ./cr3lookup/offset && make

cd .. && make

### 3.3 getting a ROP demo and run it

### 4. Display it!

cd ./getting_lbrinfo

./autogen1s-show.sh


