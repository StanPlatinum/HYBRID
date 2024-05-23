# Detecting ROP (and other) attacks from outside


This is an in-guest ROP detection method out-of-the-box, using Intel LBR. Check our [paper](https://ieeexplore.ieee.org/document/9796471) for more details.

--------------------------------------------------------------

Usage:

After `make` and `make install`, you (as the cloud provider) could use the commands in the folder which contains LibVMI-based programs to invoke your own CFI or other detection. Other VMI-IDS approaches (e.g. CAPT and CMonitor) are also supported.

Useful links:

CAPT http://www.cic-chinacommunications.cn/EN/abstract/abstract613.shtml

CMonitor https://rd.springer.com/article/10.1007/s11859-014-1030-4

# Quick start

## Install modified Xen provided by us

For now, installing Xen 4.6 seems a bit inappropriate because the version is too old, but we only have it.

Xen installing is a trivial thing but time-consuming, you may want to get some help, you could refer to https://github.com/xulai1001/auto-deploy and https://github.com/xulai1001/auto-deploy/blob/master1/rake/xen.rb.

For more about installing Xen, see https://wiki.xenproject.org/wiki/Compiling_Xen_From_Source.

Issues are welcomed.

After finishing Xen installation, you might want to get a Linux (Ubuntu is the best I guess...) guest OS.

## Install LibVMI

In our project, we use LibVMI version 0.10.1 to run some demos.

For more about installing LibVMI, see https://github.com/libvmi/libvmi.

## Run it!

### Creating HVM guest

Please also refer to Xen homepage..., https://wiki.xenproject.org/wiki/Xen_Project_Beginners_Guide.

### Using LibVMI

cd ./cr3lookup/offset && make

cd .. && make

### Getting a ROP demo and running it

### Display it!

cd ./getting_lbrinfo

./autogen1s-show.sh


