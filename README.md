# ROP-detection-in-VM

This is derived from a malware detection project from our lab.

Shortly, I build a kBouncer to Xen for detecting ROP attack in guest-VMs using Intel LBR.

--------------------------------------------------------------

Usage:

After make and make install, you (the clould provider) can use the commands in directory which contains some LibVMI-based programs to invoke your own CFI or other detection program. You can also use ours (e.g. CAPT and CMonitor).

Useful links:

CAPT http://www.cic-chinacommunications.cn/EN/abstract/abstract613.shtml
CMonitor https://rd.springer.com/article/10.1007/s11859-014-1030-4
A paper in Chinese: http://jst.tsinghuajournals.com/CN/10.16511/j.cnki.qhdxxb.2018.26.008 (please ignore the gramma glitches in the abstract which are not introduced by us authors...)

# Detailed guild:

## 1. install modified Xen provided by us

For now, install Xen 4.6 seems a bit of inappropriate because the version is too old, but we only have it.

Xen installing is a trivial thing but time-consuming, you may want to get some help from https://github.com/xulai1001/auto-deploy and https://github.com/xulai1001/auto-deploy/blob/master1/rake/xen.rb

Issues are welcomed.

After finishing Xen installing, you should get a Linux (Ubuntu is the best I guess...) as guest OS.

## 2. install libVMI

In our project, we use LibVMI version 0.10.1 to run some demos.

## 3. use it!

### 3.1 Creating a HVM guest
Please also refer to Xen homepage...

### 3.2 using LibVMI

cd ./cr3lookup

### 3.3 getting a ROP demo
