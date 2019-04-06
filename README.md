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

Detailed guild:

1. install modified Xen provided by us

2. install libVMI

3. use it!
