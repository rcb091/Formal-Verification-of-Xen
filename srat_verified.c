#include <vcc.h>

#include<stdio.h>
#include<stdlib.h>

#include <xen/init.h>
#include <xen/mm.h>
#include <xen/inttypes.h>
#include <xen/nodemask.h>
#include <xen/acpi.h>
#include <xen/numa.h>
#include <xen/pfn.h>
#include <asm/e820.h>
#include <asm/page.h>
#include <asm/spec_ctrl.h>

#define MAX_NUMNODES 1000
#define NR_NODE_MEMBLKS	2000
#define NUMA_NO_NODE 0
#define NUMA_NO_DISTANCE 0
# define MAX_LOCAL_APIC 256
#define EINVAL          22  
#define ACPI_SRAT_MEM_ENABLED 1
#define ACPI_SRAT_MEM_NON_VOLATILE 1
#define ACPI_SIG_SRAT 1
#define ACPI_SRAT_TYPE_MEMORY_AFFINITY 1
#define PAGE_SHIFT	13




struct acpi_table_slit  {
	unsigned entry [1];
	bool valid;
	unsigned header;
	unsigned locality_count;
	_(invariant valid == 0 || valid == 1)
};

struct node {
	unsigned start;
	unsigned end;
  _(invariant start <= end)
};

struct acpi_srat_x2apic_cpu_affinity{
	unsigned header;
	int flags;
	int apic_id;
	unsigned proximity_domain;
  _(invariant apic_id > 0)
  _(invariant header > 0)
  };
  
 struct acpi_srat_cpu_affinity{
	unsigned header;
	int flags;
	int apic_id;
	unsigned proximity_domain_lo;
	unsigned proximity_domain_hi[3];
   _(invariant apic_id > 0)
  _(invariant header > 0)
 };
  
 struct acpi_srat_mem_affinity{
	unsigned header;
	int flags;
	unsigned base_address;
	unsigned length;
    unsigned proximity_domain;
     _(invariant length > 0)
  _(invariant header > 0)
 };
  
struct pxm2node {
	unsigned pxm;
	int node;
  _(invariant pxm > 0)
};

c

struct e820map{
	int nr_map;
	struct map map[10]; 
 _(invariant nr_map >0)
 };


int acpi_numa;
int apicid_to_node[256];
int srat_rev;
unsigned mem_hotplug;
unsigned srat_region_mask;
static unsigned memory_nodes_parsed ;
static unsigned processor_nodes_parsed  ;
static bool node_test_and_set(unsigned node , unsigned parsed);
static struct node nodes[MAX_NUMNODES]  ;
int acpi_numa;
int apicid_to_node[256];
int srat_rev;
unsigned mem_hotplug;
unsigned srat_region_mask;
static int num_node_memblks;
static unsigned memblk_nodeid[NR_NODE_MEMBLKS];

static bool numa_off();
static bool acpi_disabled();
static bool acpi_parse_srat();
static bool acpi_table_parse(int a, bool b);
static bool test_bit(int a, int b);
static  void bad_srat(void);
static  bool srat_disabled(void);
void memcpy(struct acpi_table_slit a, struct acpi_table_slit* b, int n);
static unsigned node_to_pxm(unsigned n);
int PFN_UP(int length);
static struct acpi_table_slit mfn_to_virt(int n);
int mfn_x(int n);
static void numa_init_array();
void setup_node_bootmem(int a, unsigned start, unsigned end);
bool pdx_region_mask(unsigned addr,unsigned size);
void pfn_pdx_hole_setup(unsigned mask);
bool node_isset(int a, unsigned nodes);
void numa_set_node(int i, int node);
static unsigned pdx_init_mask(unsigned addr);

static struct pxm2node pxm2node[MAX_NUMNODES];
static struct acpi_table_slit  acpi_slit;
static struct node node_memblk_range[NR_NODE_MEMBLKS];
struct e820map e820;
struct e820map e820_saved;

//static  data DECLARE_BITMAP(memblk_hotplug, NR_NODE_MEMBLKS);
unsigned alloc_boot_pages(int length, int n);


 inline bool node_found(unsigned idx, unsigned pxm)
{
	_(assume \writable(&pxm2node[idx].pxm))
	_(assume \writable(&pxm2node[idx].node))
	return ((pxm2node[idx].pxm == pxm) &&	(pxm2node[idx].node != 0));
}

unsigned pxm_to_node(unsigned pxm)
_(requires \thread_local_array(pxm2node, sizeof(pxm2node)))
_(requires \thread_local(&pxm2node[pxm]))
_(requires \forall unsigned i; \thread_local(&pxm2node[i]))
_(writes \array_range((&pxm2node[pxm]), MAX_NUMNODES))
_(ensures \result >=0)
{
	unsigned i;
	if ((pxm < sizeof(pxm2node)) && node_found(pxm, pxm))
		return _(unchecked) (unsigned)pxm2node[pxm].node;
	for (i = 0; i < sizeof(pxm2node); i++)
		if (node_found(i, pxm))
    {
    	_(assume \thread_local(&pxm2node[i].node))
		return _(unchecked)  (unsigned)pxm2node[i].node;
    }
	return 0;
}


unsigned setup_node(unsigned pxm)
_(requires \thread_local_array(pxm2node, sizeof(pxm2node)))
_(requires \thread_local(&pxm2node[pxm]))
_(writes \array_range((&pxm2node[pxm]), MAX_NUMNODES))
_(requires \forall unsigned idx; \thread_local(&pxm2node[idx]))
_(ensures \result >=0)
{
	unsigned node;
	unsigned idx;
	static bool warned;
	static unsigned nodes_found;
  	//BUILD_BUG_ON(MAX_NUMNODES >= NUMA_NO_NODE);
	if (pxm < sizeof(pxm2node)) {
		if (node_found(pxm, pxm))
			return _(unchecked) (unsigned) pxm2node[pxm].node;

		/* Try to maintain indexing of pxm2node by pxm */
		if (pxm2node[pxm].node == NUMA_NO_NODE) {
			idx = pxm;
			goto finish;
		}
	}

	for (idx = 0; idx < sizeof(pxm2node); idx++)
		if (pxm2node[idx].node == NUMA_NO_NODE)
			goto finish;

	if (!warned) {
		//printk(KERN_WARNING "SRAT: Too many proximity domains (%#x)\n",  pxm);
		warned = 1;
	}

	return NUMA_NO_NODE;

 finish:
	node = _(unchecked) nodes_found++;
	if (node >= MAX_NUMNODES)
		return NUMA_NO_NODE;
   
  _(assume \writable(&pxm2node[idx].pxm))
	pxm2node[idx].pxm = pxm;
	pxm2node[idx].node =  (int)node;

	return node;
}

int valid_numa_range(unsigned start, unsigned end, unsigned node)
_(requires \thread_local(&num_node_memblks))
_(requires \thread_local_array(nodes, sizeof(nodes)))
_(ensures (\result == 1) || (\result == 0))
{
	int i;

	for (i = 0; i < num_node_memblks; i++) {
		struct node *nd = &node_memblk_range[i];
    	 _(assume \thread_local(nd)) 
    	 _(assume \forall int i; \thread_local(&memblk_nodeid[i]))
		if (nd->start <= start && nd->end >= end &&  memblk_nodeid[i] == node)
			return 1;
	}

	return 0;
}

static   int conflicting_memblks(unsigned start, unsigned end)
{
	int i;
	_(assume \thread_local(&num_node_memblks))
	for (i = 0; i < num_node_memblks; i++) {
		struct node *nd = &node_memblk_range[i];
    	_(assume \thread_local(nd))
		if (nd->start == nd->end)
			continue;
		if (nd->end > start && nd->start < end)
			return i;
		if (nd->end == end && nd->start == start)
			return i;
	}
	return -1;
}

static   void cutoff_node(int i, unsigned start, unsigned end)
{
 
	struct node *nd = &nodes[i];
	_(assume \thread_local(nd))
	_(assume \writable(&(nd->start)))
	_(assume \writable(&(nd->end)))
	if (nd->start < start) {
		nd->start = start;
		if (nd->end < nd->start)
			nd->start = nd->end;
	}
	if (nd->end > end) {
		nd->end = end;
		if (nd->start > nd->end)
			nd->start = nd->end;
	}
}

static   void bad_srat(void)
_(writes \array_range(pxm2node, sizeof(pxm2node)))
_(writes \array_range(apicid_to_node, sizeof(apicid_to_node)))
{
	int mem_hotplug;
	//printk(KERN_ERR "SRAT: SRAT not used.\n");
	acpi_numa = -1;
	int i;
  	for (i = 0; i < 256; i++)
		apicid_to_node[i] = NUMA_NO_NODE;
	for (i = 0; i < sizeof(pxm2node); i++)
	pxm2node[i].node = NUMA_NO_NODE;
	mem_hotplug = 0;
}

/*
 * A lot of BIOS fill in 10 (= no distance) everywhere. This messes
 * up the NUMA heuristics which wants the local node to have a smaller
 * distance than the others.
 * Do some quick checks here and only use the SLIT if it passes.
 */


static   int slit_valid(struct acpi_table_slit *slit)
_(writes slit)
_(writes \extent(slit))
{
	unsigned i, j;
	unsigned d =  slit->locality_count;
	for (i = 0; i < _(unchecked)  d; i++) {
		for (j = 0; j <_(unchecked) d; j++)  {
    		unsigned a =_(unchecked) ( d*i + j) ;
    		_(assume \thread_local(& slit->entry[a]))
			unsigned val =  slit->entry[a];
			if (i == j) {
				if (val != 10)
					return 0;
			} else if (val <= 10)
				return 0;
		}
	}
	return 1;
}

/* Callback for SLIT parsing */
void   acpi_numa_slit_init(struct acpi_table_slit *slit)
_(writes &acpi_slit)
_(writes \extent(&acpi_slit))
{
	int mfn;
	//if (!slit_valid(slit)) {
	//printk(KERN_INFO "ACPI: SLIT table looks invalid. " "Not used.\n");
	//	return;
	//}
	mfn =_(unchecked) (int)alloc_boot_pages(PFN_UP(sizeof(slit->header)), 1);
 	acpi_slit = mfn_to_virt(mfn_x(mfn));
	memcpy(acpi_slit, slit, sizeof(slit->header));
}

/* Callback for Proximity Domain -> x2APIC mapping */
void  acpi_numa_x2apic_affinity_init(const struct acpi_srat_x2apic_cpu_affinity *pa)
_(writes \array_range(pxm2node, sizeof(pxm2node)))
_(writes \array_range(apicid_to_node, sizeof(apicid_to_node)))
{
	unsigned pxm;
	unsigned node;
	if (srat_disabled())
		return;
	if (sizeof(pa->header) < sizeof(struct acpi_srat_x2apic_cpu_affinity)) {
		bad_srat();
		return;
	}
	int ACPI_SRAT_CPU_ENABLED = 1;
	if (!(pa->flags & ACPI_SRAT_CPU_ENABLED))
		return;
    int MAX_LOCAL_APIC=256;
	if (pa->apic_id >= MAX_LOCAL_APIC) {
		//printk(KERN_INFO "SRAT: APIC %08x ignored\n", pa->apic_id);
		return;
	}
	pxm = pa->proximity_domain;
	node = setup_node(pxm);
	if (node == NUMA_NO_NODE) {
		bad_srat();
		return;
	}
	apicid_to_node[pa->apic_id] =(int) node;
	//node_set(node, processor_nodes_parsed);
	acpi_numa = 1;
	//printk(KERN_INFO "SRAT: PXM %u -> APIC %08x -> Node %u\n",pxm, pa->apic_id, node);
}

/* Callback for Proximity Domain -> LAPIC mapping */
void acpi_numa_processor_affinity_init(const struct acpi_srat_cpu_affinity *pa)
_(writes \array_range(pxm2node, sizeof(pxm2node)))
_(writes \array_range(apicid_to_node, sizeof(apicid_to_node)))
{
	unsigned pxm;
	unsigned node;
	if (srat_disabled())
		return;
	if (sizeof(pa->header) != sizeof(struct acpi_srat_cpu_affinity)) {
		bad_srat();
		return;
	}
  	int  ACPI_SRAT_CPU_ENABLED=1;
	if (!(pa->flags && ACPI_SRAT_CPU_ENABLED))
		return;
	pxm = pa->proximity_domain_lo;
  
	if (srat_rev >= 2) {
		pxm |= pa->proximity_domain_hi[0] << 8;
		pxm |= pa->proximity_domain_hi[1] << 16;
		pxm |= pa->proximity_domain_hi[2] << 24;
	}
	node = setup_node(pxm);
	if (node == NUMA_NO_NODE) {
		bad_srat();
		return;
	}
	apicid_to_node[pa->apic_id] = (int)node;
	//node_set(node, processor_nodes_parsed);
	acpi_numa = 1;
	//printk(KERN_INFO "SRAT: PXM %u -> APIC %02x -> Node %u\n", pxm, pa->apic_id, node);
}

/* Callback for parsing of the Proximity Domain <-> Memory Area mappings */
void acpi_numa_memory_affinity_init(const struct acpi_srat_mem_affinity *ma)
_(writes \array_range(pxm2node, sizeof(pxm2node)))
_(writes \array_range(apicid_to_node, sizeof(apicid_to_node)))
{
	unsigned start, end;
	unsigned pxm;
	unsigned node;
	int i;

	if (srat_disabled())
		return;
	if (sizeof(ma->header) != sizeof(struct acpi_srat_mem_affinity)) {
    
		bad_srat();
		return;
	}
  
	if (!(ma->flags & ACPI_SRAT_MEM_ENABLED))
		return;

	start = ma->base_address;
	end = start + ma->length;
	/* Supplement the heuristics in l1tf_calculations(). */
	//l1tf_safe_maddr = max(l1tf_safe_maddr, ROUNDUP(end, PAGE_SIZE));

	if (num_node_memblks >= NR_NODE_MEMBLKS)
	{
		//printk(XENLOG_WARNING,"Too many numa entry, try bigger NR_NODE_MEMBLKS \n")
		bad_srat();
		return;
	}

	pxm = ma->proximity_domain;
	if (srat_rev < 2)
		pxm &= 0xff;
	node = setup_node(pxm);
	if (node == NUMA_NO_NODE) {
		bad_srat();
		return;
	}


	/* It is fine to add this area to the nodes data it will be used later*/
	i = conflicting_memblks(start, end);
    int ACPI_SRAT_MEM_HOT_PLUGGABLE=1;
    int memblk_hotplug;
	if (memblk_nodeid[i] == node) {
		bool mismatch = !(ma->flags & ACPI_SRAT_MEM_HOT_PLUGGABLE) != !test_bit(i, memblk_hotplug);

		//printk("%sSRAT: PXM %u (%"PRIx64"-%"PRIx64") overlaps with itself (%"PRIx64"-%"PRIx64")\n",   mismatch ? KERN_ERR : KERN_WARNING, pxm, start, end, node_memblk_range[i].start, node_memblk_range[i].end);
		if (mismatch) {
			bad_srat();
			return;
		}
	} 
	else {
		//printk(KERN_ERR "SRAT: PXM %u (%"PRIx64"-%"PRIx64") overlaps with PXM %u (%"PRIx64"-%"PRIx64")\n", pxm, start, end, node_to_pxm(memblk_nodeid[i]),node_memblk_range[i].start, node_memblk_range[i].end);
		bad_srat();
		return;
	}
	if (!(ma->flags & ACPI_SRAT_MEM_HOT_PLUGGABLE)) {
		struct node *nd = &nodes[node];

		if (!node_test_and_set(node, memory_nodes_parsed)) {
			nd->start = start;
			nd->end = end;
		} else {
			if (start < nd->start)
				nd->start = start;
			if (nd->end < end)
				nd->end = end;
		}
	}
	//printk(KERN_INFO "SRAT: Node %u PXM %u %"PRIx64"-%"PRIx64"%s\n", node, pxm, start, end, ma->flags & ACPI_SRAT_MEM_HOT_PLUGGABLE ? " (hotplug)" : "");

	node_memblk_range[num_node_memblks].start = start;
	node_memblk_range[num_node_memblks].end = end;
	memblk_nodeid[num_node_memblks] = node;
	if (ma->flags & ACPI_SRAT_MEM_HOT_PLUGGABLE) {
		//__set_bit(num_node_memblks, memblk_hotplug);
		if (end > mem_hotplug)
			mem_hotplug = end;
	}
	num_node_memblks++;
}


/* Sanity check to catch more bad SRATs (they are amazingly common).
   Make sure the PXMs cover all memory. */
static int   nodes_cover_memory()
{
	int i;
	_(assume \thread_local(&e820))
	for (i = 0; i < e820.nr_map; i++) {
		int j, found;
		unsigned long long start, end;

		if (e820.map[i].type != "E820_RAM") {
			continue;
		}
		_(assume \thread_local(&e820.map[i].addr))
		start = e820.map[i].addr;
		end = _(unchecked) (e820.map[i].addr + e820.map[i].size);

		do {
			found = 0;
         	int memory_nodes_parsed;
   
			for(j=0;j< memory_nodes_parsed;j++)
      		{
      			_(assume \thread_local(&nodes[j].end))
      			_(assume \thread_local(&nodes[j].start))
				if (start < nodes[j].end  && end > nodes[j].start) 
        		{
					if (start >= nodes[j].start) {
						start = nodes[j].end;
						found = 1;
					}
					if (end <= nodes[j].end) {
						end = nodes[j].start;
						found = 1;
					}
				}
			}
		} while (found && start < end);

		if (start < end) {
			//printk(KERN_ERR "SRAT: No PXM for e820 range: "	"%016Lx - %016Lx\n", start, end);
			return 0;
		}
	}
	return 1;
}

void   acpi_numa_arch_fixup(void);

static unsigned srat_region_mask();

static int   srat_parse_region(struct acpi_subtable_header *header, const unsigned long end)
_(requires \thread_local(header))

{
	struct acpi_srat_mem_affinity *ma;
	_(assume \thread_local(ma))
	if (!header)
		return -EINVAL;
	//ma = container_of(header, acpi_srat_mem_affinity, header);
	if (!ma->length ||    !(ma->flags & ACPI_SRAT_MEM_ENABLED) ||  (ma->flags & ACPI_SRAT_MEM_NON_VOLATILE))
		return 0;
	if (numa_off())
		//printk(KERN_INFO "SRAT: %013"PRIx64"-%013"PRIx64"\n",/ma->base_address, ma->base_address + ma->length - 1);
	//srat_region_mask = ma->base_address | pdx_region_mask(ma->base_address, ma->length);
	return 0;
}

void   srat_parse_regions(unsigned addr)
_(requires \thread_local(&acpi_numa) )
_(requires \thread_local(&e820) )

{
	unsigned mask;
	unsigned int i;
	int *acpi_numa;
	_(assume \thread_local(acpi_numa))
	if (acpi_disabled()|| acpi_numa< 0 || acpi_table_parse(ACPI_SIG_SRAT, acpi_parse_srat()))
		return;
	unsigned srat_region_mask = pdx_init_mask(addr);
	//	acpi_table_parse_srat(ACPI_SRAT_TYPE_MEMORY_AFFINITY, srat_parse_region, 0);
  	_(assume \thread_local(&e820)) 
  	unsigned len = _(unchecked) (unsigned) e820.nr_map;
	for (mask = srat_region_mask, i = 0; mask && i <  len; i++) {
		if (e820.map[i].type != "E820_RAM")
			continue;
     _(assume \thread_local(&e820.map[i].addr))
		if (~mask & pdx_region_mask(e820.map[i].addr, e820.map[i].size))
			mask = 0;
	}
	pfn_pdx_hole_setup(mask >> PAGE_SHIFT);
}


/* Use the information discovered above to actually set up the nodes. */
int   acpi_scan_nodes(unsigned start, unsigned end)
_(requires \thread_local(nodes))
_(requires \thread_local(&acpi_numa))
_(writes \array_range(pxm2node, sizeof(pxm2node)))
_(writes \array_range(apicid_to_node, sizeof(apicid_to_node)))
_(ensures \result >=-1)
{
	int i;
	//nodemask_t all_nodes_parsed;
 	_(requires \thread_local(&acpi_numa))
	/* First clean up the node list */
	for (i = 0; i < MAX_NUMNODES; i++)
		cutoff_node(i, start, end);

	if (acpi_numa <= 0)
		return -1;

	if (!nodes_cover_memory()) {
		bad_srat();
		return -1;
	}
  unsigned memnode_shift;
	// memnode_shift = compute_hash_shift(node_memblk_range, num_node_memblks,	memblk_nodeid);

	if (memnode_shift < 0) {
		//printk(KERN_ERR "SRAT: No NUMA node hash function found. Contact maintainer\n");
		bad_srat();
		return -1;
	}

	//nodes_or(all_nodes_parsed, memory_nodes_parsed, processor_nodes_parsed);

	/* Finally register nodes */
 	 int all_nodes_parsed;
  
	for(i=0;i< all_nodes_parsed ;i++)
	{ 
    _(assume \thread_local(&nodes[i].end))
    _(assume \thread_local(&nodes[i].start))
		unsigned size = _(unchecked) (nodes[i].end - nodes[i].start);
		if ( size == 0 )
			//printk(KERN_WARNING "SRAT: Node %u has no memory. ""BIOS Bug or mis-configured hardware?\n", i);
		setup_node_bootmem(i, nodes[i].start, nodes[i].end);
	}
  	int nr_cpu_ids;
  	int cpu_to_node[10];
  	unsigned *a=&processor_nodes_parsed;
  
 
	for (i = 0; i < nr_cpu_ids; i++) {
    _(assume \thread_local(&cpu_to_node[i]))
		if (cpu_to_node[i] == NUMA_NO_NODE)
			continue;
      	_(assume \thread_local(a))
		if (!node_isset(cpu_to_node[i], processor_nodes_parsed))
			numa_set_node(i, NUMA_NO_NODE);
	}
	numa_init_array();
	return 0;
}

static unsigned node_to_pxm(int n)
_(requires \thread_local_array(pxm2node, sizeof(pxm2node)))
_(requires \thread_local(&pxm2node[n]))
_(requires \forall unsigned idx; \thread_local(&pxm2node[idx]))
_(ensures \result >=0)
{
	unsigned i;
 	 _(assume \true)
	if ((n < sizeof(pxm2node)) && (pxm2node[n].node == n))
		return pxm2node[n].pxm;
	for (i = 0; i < sizeof(pxm2node); i++)
		if (pxm2node[i].node == n)
			return pxm2node[i].pxm;
	return 0;
}

unsigned __node_distance(unsigned a, unsigned b)
_(writes &acpi_slit)
_(writes \extent(&acpi_slit))
_(ensures \result >=0)
{
	unsigned index;
	unsigned slit_val;
	if ( acpi_slit.valid==0)
		return a == b ? 10 : 20;
	//index = acpi_slit.locality_count * node_to_pxm(a);
  	_(assume \thread_local_array(acpi_slit.entry, sizeof(acpi_slit.entry)))
	slit_val = _(unchecked) acpi_slit.entry[index + node_to_pxm(b)];
	/* ACPI defines 0xff as an unreachable node and 0-9 are undefined */
	if ((slit_val == 0xff) || (slit_val <= 9))
		return NUMA_NO_DISTANCE;
	else
		return slit_val;
}

//EXPORT_SYMBOL(__node_distance);
