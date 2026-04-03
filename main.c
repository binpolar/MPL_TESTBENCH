#include <stdio.h>
#include "MPL/mpl_utils.h"
#include "MPL/mpl_hashmap.h"
#include "MPL/mpl_heap.h"
#include "MPL/MPL.h"

void test_edges_creation(uint32_t addr1, uint32_t addr2);
void test_nodes_creation(uint32_t addr);
void test_pathfinding_p2p(uint32_t n1, uint32_t n2);
void test_pathfinding_no_diff();
void test_pathfinding_high_diff();

int main()
{
    printf("---------MPL TESTBENCH---------\n\r");

    init_maps();
    init_heap();

    test_nodes_creation(0x0);
    test_nodes_creation(0x1);

    test_edges_creation(0x0, 0x1);

    test_pathfinding_p2p(0x0, 0x1);
    test_pathfinding_no_diff();
    test_pathfinding_high_diff();

    uint16_t tmp = mpl_route_count();
    if (tmp != 3)
    {
        printf("---------ERROR EXPECTED ROUTE COUNT 3 GOT %u---------\n\r", tmp);
    }

    printf("---------END TEST, GOT %u ROUTES---------\n\r", tmp);

    return 0;
}

mpl_route_t *find_route_and_check_route_map(mpl_node_t *src, mpl_node_t *dest)
{
    mpl_route_t *route = mpl_find_route(src, dest);
    if (route != NULL)
    {
        if (route == mpl_get_route(dest->address))
        {
            return route;
        }
    }
    return NULL;
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

void test_pathfinding_p2p(uint32_t n1, uint32_t n2)
{
    printf("---------TESTING PATHFINDING POINT 2 POINT---------\n\r");

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

    mpl_route_t *route = find_route_and_check_route_map(src, dest);
    if (route != NULL)
    {
        for (uint8_t i = 0; i < route->hop_count; i++)
        {
            printf("Hop %u:%u\n\r", i, route->hops[i]);
        }
    }
    else
    {
        printf("Pathfinding failed!!!\n\r");
    }
}

// CREATE NETWORK WHERE N1--HI-COST-------->N2
//                        \->TMP1-LOW-COST/
//
void test_pathfinding_no_diff()
{
    printf("---------TESTING PATHFINDING WITH I-MEDIATE STEP WHICH SAVES COST COMPARED TO DIRECT ROUTE---------\n\r");

    uint32_t addr1 = 1, addr2 = 2, add3 = 3;

    mpl_create_node_if_not_exists(addr1);
    mpl_create_node_if_not_exists(addr2);
    mpl_create_node_if_not_exists(add3);

    mpl_node_t *src = mpl_get_node(addr1);
    mpl_node_t *dest = mpl_get_node(addr2);
    mpl_node_t *tmp1 = mpl_get_node(add3);

    mpl_create_or_update_edge(src, dest, 100);
    mpl_create_or_update_edge(dest, src, 100);

    mpl_create_or_update_edge(src, tmp1, 10);
    mpl_create_or_update_edge(tmp1, dest, 10);

    if (src == NULL || dest == NULL || tmp1 == NULL)
    {
        printf("Couldnt get end nodes!!!\n\r");
    }

    mpl_route_t *route = find_route_and_check_route_map(src, dest);
    if (route != NULL)
    {
        for (uint8_t i = 0; i < route->hop_count; i++)
        {
            printf("Hop %u:%u\n\r", i, route->hops[i]);
        }
    }
    else
    {
        printf("Pathfinding failed!!!\n\r");
    }
}

void test_pathfinding_high_diff()
{
    printf("---------TEST: SMALL VERIFIED GRAPH---------\n\r");

    for (int i = 1; i <= 5; i++)
    {
        mpl_create_node_if_not_exists(i);
    }

    mpl_node_t *S = mpl_get_node(1); // Source
    mpl_node_t *T = mpl_get_node(5); // Target
    mpl_node_t *A = mpl_get_node(2);
    mpl_node_t *B = mpl_get_node(3);
    mpl_node_t *C = mpl_get_node(4);

    printf("Graph structure:\n\r");
    printf("   2\n\r");
    printf("  / \\\n\r");
    printf(" 1   3---5\n\r");
    printf("  \\   /\n\r");
    printf("   4\n\r\n\r");

    // Set edges with unique, non-ambiguous costs
    mpl_create_or_update_edge(S, A, 10); // 1->2 = 10
    mpl_create_or_update_edge(A, B, 10); // 2->3 = 10
    mpl_create_or_update_edge(B, T, 10); // 3->5 = 10 (total path 1: 30)

    mpl_create_or_update_edge(S, C, 20); // 1->4 = 20
    mpl_create_or_update_edge(C, B, 10); // 4->3 = 10
    // Path through 4: 1->4->3->5 = 20+10+10 = 40

    mpl_create_or_update_edge(S, B, 25); // 1->3 = 25
    // Direct: 1->3->5 = 25+10 = 35

    mpl_create_or_update_edge(B, C, 5);  // 3->4 = 5 (creates cycle)
    mpl_create_or_update_edge(C, A, 15); // 4->2 = 15

    // Calculate ALL possible paths:
    printf("\n\rAll possible paths from 1 to 5:\n\r");
    printf("  Path A: 1->2->3->5 = 10+10+10 = 30\n\r");
    printf("  Path B: 1->3->5 = 25+10 = 35\n\r");
    printf("  Path C: 1->4->3->5 = 20+10+10 = 40\n\r");
    printf("  Path D: 1->2->3->4->3->5 = 10+10+5+10+10 = 45 (cycle)\n\r");
    printf("  Path E: 1->4->2->3->5 = 20+15+10+10 = 55\n\r");
    printf("\n\rOPTIMAL PATH: 1->2->3->5 with cost 30\n\r");

    mpl_route_t *route = find_route_and_check_route_map(S, T);
    if (route != NULL)
    {
        printf("\n\rFound path (%u hops, cost: %u):\n\r", route->hop_count, route->total_cost);
        for (uint8_t i = 0; i < route->hop_count; i++)
        {
            printf("  Hop %u: Node %u\n\r", i, route->hops[i]);
        }

        if (route->total_cost == 30)
        {
            printf("\n\rFound optimal path with cost 30!\n\r");
        }
        else if (route->total_cost < 30)
        {
            printf("\n\rIMPOSSIBLE! Found cost %u < minimum possible 30!\n\r", route->total_cost);
        }
        else
        {
            printf("\n\rSUBOPTIMAL! Expected 30, got %u\n\r", route->total_cost);
        }
    }
}