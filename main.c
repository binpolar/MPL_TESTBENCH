#include <stdio.h>
#include "MPL/mpl_utils.h"
#include "MPL/mpl_hashmap.h"
#include "MPL/MPL.h"

void test_edges();

int main()
{
    printf("---------MPL TESTBENCH---------\n\r");

    init_maps();

    test_edges(0x0, 0x1);
    test_edges(0x2, 0x3);

    return 0;
}

void test_edges(uint32_t addr1, uint32_t addr2)
{
    printf("---------TESTING EDGE CREATION---------\n\r");
    mpl_node_t tmp1, tmp2;
    mpl_edge_t *edge, *egde;

    tmp1.address = addr1;
    tmp2.address = addr2;

    if (mpl_create_or_update_edge(&tmp1, &tmp2, 100))
    {
        printf("Created edge between n1 and n2, checking if it exists: %s\n\r", mpl_edge_exists(&tmp1, &tmp2) ? "SUCCESS" : "FAILED!!!");
        edge = mpl_get_edge(&tmp2, &tmp1);
        if (edge != NULL)
        {
            printf("Edge values\n\r N1:%u, N2:%u, Q:%u\n\r", edge->nodes[0], edge->nodes[1], edge->link_quality);
        }
        else
        {
            printf("COULDNT READ BACK EDGE!!!\n\r");
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
            printf("Edge update %s", egde->link_quality == 0 ? "SUCCESS" : "FAILED!!!");
        }
    }
}