#include <stdio.h>
#include "MPL/mpl_utils.h"
#include "MPL/mpl_hashmap.h"
#include "MPL/mpl_heap.h"
#include "MPL/MPL.h"

void test_edges_creation(uint32_t addr1, uint32_t addr2);
void test_nodes_creation(uint32_t addr);
void test_pathfinding(uint32_t n1, uint32_t n2);

int main()
{
    printf("---------MPL TESTBENCH---------\n\r");

    init_maps();
    init_heap();

    test_nodes_creation(0x0);
    test_nodes_creation(0x1);

    test_edges_creation(0x0, 0x1);

    test_pathfinding(0x0, 0x1);

    return 0;
}

void test_nodes_creation(uint32_t addr)
{
    printf("---------TESTING NODE CREATION---------\n\r");
    if (mpl_create_node_if_not_exists(addr))
    {
        printf("Created nod, checking if it exists: %s\n\r", mpl_node_exists(addr) ? "SUCCESS" : "FAILED!!!");
    }
    else
    {
        printf("Node creation failed!!!");
    }
}

void test_edges_creation(uint32_t addr1, uint32_t addr2)
{
    printf("---------TESTING EDGE CREATION---------\n\r");
    mpl_node_t tmp1 = {0}, tmp2 = {0};
    mpl_edge_t *edge, *egde;

    tmp1.address = addr1;
    tmp2.address = addr2;

    if (mpl_create_or_update_edge(&tmp1, &tmp2, 100))
    {
        printf("Created edge between n1 and n2, checking if it exists: %s\n\r", mpl_edge_exists(&tmp1, &tmp2) ? "SUCCESS" : "FAILED!!!");
        edge = mpl_get_edge(&tmp2, &tmp1);
        if (edge != NULL)
        {
            printf("Edge values\n\r N1:%u, N2:%u, Q:%u\n\r", edge->nodes[0]->address, edge->nodes[1]->address, edge->link_quality);
        }
        else
        {
            printf("Couldnt read back edge!!!\n\r");
        }
    }
    else
    {
        printf("Edge creation failed!!!\n\r");
    }
    printf("---------TESTING EDGE UPDATING---------\n\r");

    if (mpl_create_or_update_edge(&tmp2, &tmp1, 0))
    {
        egde = mpl_get_edge(&tmp1, &tmp2);
        if (edge != egde)
        {
            printf("Got two instances in memory instead of just one!!!\n\r");
        }
        else
        {
            printf("Edge update %s\n\r", egde->link_quality == 0 ? "SUCCESS" : "FAILED!!!");
        }
    }
}

void test_pathfinding(uint32_t n1, uint32_t n2)
{
    printf("---------TESTING PATHFINDING---------\n\r");

    mpl_create_node_if_not_exists(n1);
    mpl_create_node_if_not_exists(n2);

    mpl_node_t *src = mpl_get_node(n1);
    mpl_node_t *dest = mpl_get_node(n2);

    mpl_create_or_update_edge(src, dest, 100);
    mpl_create_or_update_edge(dest, src, 100);

    if (src == NULL || dest == NULL)
    {
        printf("Couldnt get end nodes!!!\n\r");
    }

    mpl_route_t tmp;
    if (mpl_find_route(src, dest, &tmp))
    {
        for (uint8_t i = 0; i < tmp.hop_count; i++)
        {
            printf("Hop %u:%u\n\r", i, tmp.hops[i]);
        }
    }
    else
    {
        printf("Pathfinding failed!!!\n\r");
    }
}