#ifndef _LB_COMMON_H
#define _LB_COMMON_H
#include <inttypes.h>
#include <e3_log.h>
#include <util.h>
#include <urcu-qsbr.h>
#define __key
/*all data structure is in network byte order*/
struct virtual_ip{
	union{/*VIP structure*/
		uint32_t __key ip_as_u32;
		uint16_t ip_as_u16[2];
		uint8_t ip_as_u8[4];
	};
	uint32_t next_hop_ip;
	uint8_t next_mac[6];/*only ARP-snooping can do modify mac*/
	union{/*external network vlan insertion is supported,
	once vlan_vid is set 0,no vlan header is inserted*/
		uint16_t vlan_tci;
		struct{
			uint16_t vlan_pri:3;
			uint16_t vlan_cfi:1;
			uint16_t vlan_vid:12;
		};
	};
	
	struct rcu_head rcu;
	void (*vip_reclaim_function)(struct rcu_head*);
	uint16_t local_index;
	uint16_t phy_if_index;
	uint16_t vir_if_index;
	uint16_t lb_instance_index;
	
}__attribute__((aligned(64)));

struct real_server{
	uint32_t rs_host_ipv4;
	uint32_t rs_ipv4;/*it supports DSR and NAT mode,so make real_server address clear*/
	uint32_t __key tunnel_id:24;/*project network vxlan segment ID*/
	uint8_t __key rs_mac[6];/*vNIV mac create by neutron port*/
	uint8_t  rs_host_mac[6];
	uint16_t lb_iface;/*VM routes traffic to LB by routing traffic to 
	the neutron port where an entity will delegate the basic L3 functions*/

	uint16_t local_index;
	struct rcu_head rcu;
	void (*real_server_rcu_reclaim_func)(struct rcu_head *);
};

#define L3_INTERFACE_TYPE_VIRTUAL 0x1
#define L3_INTERFACE_TYPE_PHYSICAL 0x2


struct l3_interface{
	union{
		uint32_t __key if_ip_as_u32;
		uint16_t if_ip_as_u16[2];
		uint8_t if_ip_as_u8[4];
	};
	uint8_t if_mac[6];
	uint8_t if_type;
	uint16_t __key lower_if_index;/*if it's a virtual interface ,the lower_if_index
	is the physical E3Interface,otherwise it's another virtual interface*/
	uint16_t local_index;
	
	struct rcu_head rcu;
	void (*l3iface_reclaim_function)(struct rcu_head*);
}__attribute__((aligned(64)));
#define MAX_MEMBER_LENTTH 256

struct lb_instance{
	
	uint16_t vip_index;
	uint16_t members[MAX_MEMBER_LENTTH];
};
#define as_u8(oct) (0xff&(uint8_t)(oct))
#define as_u32(oct) (0xffffffff&(uint32_t)(oct))


#define MAKE_IP32(oct1,oct2,oct3,oct4) (as_u32(oct1)|\
	(as_u32(oct2)<<8)|\
	(as_u32(oct3)<<16)|\
	(as_u32(oct4)<<24))

#endif