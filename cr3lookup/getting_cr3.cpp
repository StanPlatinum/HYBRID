#include "cstdio"
#include "cstdlib"
#include "iostream"
#include "fstream"
#include "sstream"
#include "string"
#include "cstring"
#include "map"
#include "vector"
#include <time.h>

extern "C" {
#include "libvmi/libvmi.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "inttypes.h"
#include "sys/types.h"
}

using namespace std;

#define fail(retvar, msg, args...) { \
	fprintf(stderr, "FAIL: Line %d: " msg, __LINE__, ##args); return retvar; }
#define assert_success(op) if ((op)) { fail(-1, #op " ret != VMI_SUCCESS"); }
#define assert_valid_ptr(op) if (!(op)) { fail(0, #op, " ret == null"); }
#define _(op) op
#define step(n, comment, wrapper, op) { cout << "\033[32m" << n << " - " << comment << "\033[0m" << endl; wrapper(op); }

typedef unsigned long addrtype;

string ntoa(unsigned ip)
{
	in_addr in;
	in.s_addr = ip;
	string ret = inet_ntoa(in);
	return ret;
}

struct Portinfo
{
	int pid, inode;
	short uid;
	long atime, mtime, ctime;
	int64_t timestamp;
	unsigned long mem;
	string proc_name, proto_name, cmdline;
	vector<string> files;
	unsigned saddr, daddr, rcv_saddr;
	unsigned short sport, dport;

	string to_s()
	{
		int i;
		stringstream ss; ss.clear();
		ss << "[" << dec << pid << "]" << proc_name << " user #" << uid << "  Memory " << mem << "k" << " \tcmd: " << cmdline << endl;
		ss << "  open files: " << files.size();
		//       for (i=0; i<files.size(); i++)
		//           ss << files[i] << " ";
		ss << endl;
		ss << "  inode 0x" << hex << inode << dec << " " << proto_name << " " << ntoa(saddr) << " : " << sport << " <-> " << ntoa(daddr) << " : " << dport << endl;
		return ss.str();
	}

	string to_json()
	{
#define json(x) "\"" << #x << "\" : " << x << ", "
#define json_str(x) "\"" << #x "\" : \"" << x << "\", "
#define ntoa(x) "\"" << #x << "\" : " << inet_ntoa(*reinterpret_cast<in_addr *>(&(x))) << ", "
		int i; stringstream ss; ss.clear();
		ss << "{ ";

		ss << dec << json(pid) << json(inode) << json(uid) << json(mem) <<
			json_str(proc_name) << json_str(proto_name) << json_str(cmdline) << json(atime) << 
			json(ctime) << json(mtime) << ntoa(saddr) << json(sport);
		ss << dec << ntoa(rcv_saddr);
		ss << dec << ntoa(daddr) << json(dport) << json(timestamp) << endl;
		ss << "\"files\" : [ ";
		for (i=0; i<files.size(); i++)
		{
			ss << "\"" << files[i] << "\"";
			if (i<files.size()-1) ss << ",";
			ss << " ";
		}
		ss << "] }";

		return ss.str();
	}
};

class VMIHelper
{
	public:
		vmi_instance_t v;
		int pid;

	public:
		VMIHelper() : pid(0){}
		VMIHelper(string & name) : pid(0) { init(name); }
		~VMIHelper() { resume(); vmi_destroy(v); }

		void init(string & name)
		{
			assert_success( vmi_init(&v, VMI_AUTO | VMI_INIT_COMPLETE, name.c_str()) );
			cout << endl;
		}
		void init(char *name)
		{
			assert_success( vmi_init(&v, VMI_AUTO | VMI_INIT_COMPLETE, name) );
			cout << endl;
		}

		int pause() { return vmi_pause_vm(v); }
		void resume() { vmi_resume_vm(v); }

		vmi_instance_t & operator()() { return v; }

		template <typename T>
			T get(addr_t vaddr)
			{
				T ret;vmi_read_va(v, vaddr, pid, &ret, sizeof(T));
				return ret;
			}

		char *get(addr_t vaddr)
		{
			char *ret; assert_valid_ptr( ret = vmi_read_str_va(v, vaddr, pid) );
			return ret;
		}

		addrtype operator *(addrtype vaddr)
		{
			return get<addrtype>(vaddr);
		}
};

VMIHelper vmi;
char vm_name[] = "ubuntu-tracepoint";
char vm_sym[] = "./System.map-4.4.0-31-generic";
char vm_offset[] = "./tracepoint-ubuntu.offset";
map<string, int> sem;   //semantic table
vector<Portinfo> result;
Portinfo curr_entry;

void print_sem()
{
	for (map<string, int>::iterator it = sem.begin();
			it != sem.end(); ++it)
		cout << it->first << " : 0x" << hex << it->second << endl;
}

void print_result()
{
	int i;
	cout << "[ " << endl;
	for (i=0; i<result.size(); i++) {
		cout << result[i].to_json();
		if (i<result.size()-1) cout << ",";
		cout << endl;
	}
	cout << "]" << endl;
}

int load_sem(char *filename)
{
	ifstream ifs(filename);
	string key, dummy;
	int value;

	sem.clear();
	while ( ifs >> key >> dummy)
	{
		if (key.find("sizeof") != string::npos)
			ifs >> dec >> value;
		else
			ifs >> hex >> value;
		sem[key] = value;
	}
	ifs.close();
	return 0; 
}

#define vmi_for_each_process(init, p) \
	for(p = init; p = vmi*(p + sem["task_struct->tasks"]) - sem["task_struct->tasks"], p != init; )

#if 0
int process_sock(addrtype base)
{
	addrtype sock = vmi*(base + sem["socket->sk"]);
	char *name = vmi.get(vmi*(sock + sem["sock->sk_prot"]) + sem["proto->name"]);
	curr_entry.proto_name.assign(name);


	int64_t time_stamp = vmi.get<int64_t>(sock + sem["sock->sk_stamp"]) ;
	unsigned short family = vmi.get<unsigned short>(sock + sem["sock->sk_family"]);
	if (family == PF_INET)
	{
		unsigned daddr, rcv_saddr, saddr;
		unsigned short dport, sport;
		daddr = vmi.get<unsigned>(sock + sem["inet_sock->inet_daddr"]);
		rcv_saddr = vmi.get<unsigned>(sock + sem["inet_sock->inet_rcv_saddr"]);
		dport = vmi.get<unsigned short>(sock + sem["inet_sock->inet_dport"]);
		sport = vmi.get<unsigned short>(sock + sem["inet_sock->inet_sport"]);
		saddr = vmi.get<unsigned>(sock + sem["inet_sock->inet_saddr"]);

		curr_entry.rcv_saddr = rcv_saddr;
		curr_entry.saddr = saddr;
		curr_entry.daddr = daddr;
		curr_entry.sport = ntohs(sport);
		curr_entry.dport = ntohs(dport);

		int64_t timestamp = time_stamp/1000000000;
		curr_entry.timestamp = timestamp;
		cout << " 0x" << hex << sock << "; proto->name:" << name << dec << "; time_stamp: " << timestamp << endl;
		result.push_back(curr_entry);

#define ntoa(x) inet_ntoa(*reinterpret_cast<in_addr *>(&(x)))

		cout << hex << " saddr: " << ntoa(saddr) <<
			dec << " sport: " << sport;
		cout << hex << " rcv_saddr: " << ntoa(rcv_saddr);
		cout << hex << " daddr: " << ntoa(daddr) << 
			dec << " dport: " << dport << endl;

	}
	free(name);
	return 0;
}

int process_filetype(addrtype base)
{
	if (!base) return -1;
	addrtype f_inode = vmi*(base + sem["file->f_inode"]);        

	time_t access_time = vmi.get<time_t>(f_inode + sem["inode->i_atime"] + sem["timespec->tv_sec"]);
	time_t change_time = vmi.get<time_t>(f_inode + sem["inode->i_ctime"] + sem["timespec->tv_sec"]);
	time_t modifi_time = vmi.get<time_t>(f_inode + sem["inode->i_mtime"] + sem["timespec->tv_sec"]);
	struct tm *p;
	p = localtime(&access_time);                
	char s[80];
	strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);                
	cout << dec << "atime: " << access_time << "; UTC_time: " << s << endl;
	p = localtime(&change_time);
	strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);
	cout << dec << "ctime: " << change_time << "; UTC_time: " << s << endl;
	p = localtime(&modifi_time);
	strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);
	cout << dec << "mtime: " << modifi_time << "; UTC_time: " << s << endl;     

	addrtype fileop = vmi*(base + sem["file->f_op"]);
	ssize_t read = vmi.get<ssize_t>(fileop + sem["file_operations->read"]);
	ssize_t write = vmi.get<ssize_t>(fileop + sem["file_operations->write"]);
	int open = vmi.get<int>(fileop + sem["file_operations->open"]);
	int close = vmi.get<int>(fileop + sem["file_operations->release"]);
	cout << dec << "read: " << read << "; write: " << write << "; open: " << open << "; close: " << close << endl;

	addrtype dentry = vmi*(base + sem["file->f_path"] + sem["path->dentry"]);
	if(dentry)
	{
		unsigned char *d_iname = vmi.get(dentry + sem["dentry->d_iname"]);
		unsigned long d_time = vmi.get<unsigned long>(dentry + sem["dentry->d_time"]);;
		cout << dec << "d_iname: " << d_iname << "; d_time: " << d_time << endl;
	}

	return 0;
}

int process_file(addrtype base)
{
	if (!base) return -1;
	int i, count = vmi.get<int>(vmi*(base + sem["files_struct->fdt"]) + sem["fdtable->max_fds"]);
	addrtype fdarray = vmi*(vmi*(base + sem["files_struct->fdt"]) + sem["fdtable->fd"]);
	char fdset[5000] = {0};

	addrtype fds_base = vmi*(vmi*(base + sem["files_struct->fdt"]) + sem["fdtable->open_fds"]);
	vmi_read_va(vmi(), fds_base, 0, fdset, count / 8 + 1);

	for (i=0; i<count; i++)
	{
		if (fdset[i / 8] & (1 << (i % 8)))
		{
			addrtype f = vmi*(fdarray + i*sizeof(addrtype));
			if (f)
			{
				//analyse inode
				addrtype inode = vmi*(f + sem["file->f_inode"]);

				if (inode)
				{
					long id = vmi.get<long>(inode + sem["inode->i_ino"]);
					unsigned short mode = vmi.get<unsigned short>(inode + sem["inode->i_mode"]);
					if (S_ISREG(mode))
					{
						cout << dec << count << ". id: " << id << " ; regular file" << endl;
						process_filetype(f);
					}

					else if (S_ISSOCK(mode))
					{
						cout << dec << count << ". id: " << id << " ; socket communication" << endl;
						curr_entry.inode = inode;
						process_sock(vmi*(f + sem["file->private_data"]));
					}

				}
			}
		}
	}    
	return 0;
}
#endif

/* written by liuweijie, for where if pgd_t is struct */
int process_cr3(addrtype base)
{
	if (!base) return -1;
	unsigned long pgd;
	pgd = vmi.get<unsigned>(vmi*(base));
	if(pgd > 0)
		cout << hex << "pgd: 0x" << pgd << endl;

	//todo: p2v
	//todo: build a lookup table

	return 0;
}

int process()
{
	/* prepare a target file */
	ofstream fout;
	fout.open("./lookupinfo.txt");

	addrtype init_addr, proc;
	result.clear();
	vmi.pause();

	init_addr = vmi_translate_ksym2v(vmi(), "init_task");
	vmi_for_each_process(init_addr, proc) {
		int pid = vmi.get<int>(proc + sem["task_struct->pid"]);
		char *name = vmi.get(proc + sem["task_struct->comm"]);
		int real_parent_pid = vmi.get<int>(vmi*(proc + sem["task_struct->real_parent"]) + sem["task_struct->pid"]);
		char *real_parent_name = vmi.get(vmi*(proc + sem["task_struct->real_parent"]) + sem["task_struct->comm"]);
		unsigned long long start_time = vmi.get<unsigned long long>(proc + sem["task_struct->real_start_time"]);
		cout << dec << "[" << pid << "] " << name << endl;

		/* entering mm_struct */
		addrtype mm = vmi*(proc + sem["task_struct->mm"]);
		unsigned long long cr3 = 0;
		unsigned long long pgd = vmi.get<unsigned long long>(mm + sem["mm_struct->pgd"]);
		if(pgd > 0)	printf("pgd:%llx\n", pgd);

		/* you should check the offset in your own kernel. Here is ffff880000000000 */
		unsigned long long cr3_offset = 0xffff880000000000;
		/* record the info only when cr3 is valid */
		if(pgd > cr3_offset) {
			cr3 = pgd - cr3_offset;
			printf("cr3:%llx\n", cr3);

			/* write the info to lookupinfo.txt */
			fout << dec << pid << " " << hex << cr3 << endl;
		}

	} //end of vmi_for_each_process

	vmi.resume();

	fout << flush; fout.close();

	return 0;
}

int main(int argc, char **argv)
{
	// specify vm conf in /etc/libvmi.conf
	vmi.init(vm_name);
	char *name = vmi_get_name(vmi());
	unsigned long id = vmi_get_vmid(vmi());

	free(name);
	load_sem(vm_offset);
	cout << "name: " << vm_name << ", id: " << id << ", " << endl <<
		"result: " << endl;
	process();

	return 0;
}
