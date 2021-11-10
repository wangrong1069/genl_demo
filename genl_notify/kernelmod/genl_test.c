#include <linux/kernel.h>
#include <linux/module.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include <linux/skbuff.h>
#include <linux/timer.h>

#include "../genl_test.h"

static struct timer_list genl_test_timer;

/* multicast group */
enum genl_test_multicast_groups {
	GENL_TEST_MCG_0,
	GENL_TEST_MCG_1,
};

static const struct genl_multicast_group genl_test_mcgs[] = {
    [GENL_TEST_MCG_0] = { .name = GENL_TEST_MCG_NAME_0, },
    [GENL_TEST_MCG_1] = { .name = GENL_TEST_MCG_NAME_1, },
};

/* family definition */
static struct genl_family genl_test_gnl_family = {
    .name = GENL_TEST_FAMILY_NAME,
    .version = 1,
    .module = THIS_MODULE,
    .maxattr = GENL_TEST_A_MAX,
    .netnsok = false,
    .mcgrps = genl_test_mcgs,
    .n_mcgrps = ARRAY_SIZE(genl_test_mcgs),
};

static int genl_test_notify(unsigned int group)
{
    int rc;
    char buf[256];
    struct sk_buff *msg;
    void *msg_head;

    /* alloc msg */
    msg = genlmsg_new(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!msg)
		return -ENOMEM;

    /* construct msg */
    /* create the message headers */
    msg_head = genlmsg_put(msg, 0, 0, &genl_test_gnl_family, GFP_ATOMIC, GENL_TEST_C_NOTIFY);
    if (!msg_head) {
        rc = -ENOMEM;
        goto failure;
    }
    /* add a GENL_TEST_A_MSG attribute */
    snprintf(buf, sizeof(buf), "genl_test_notify: group(%u), jiffies(%u)", group, (unsigned int)jiffies);
    rc = nla_put_string(msg, GENL_TEST_A_MSG, buf);
    if (rc != 0)
        goto failure;
    /* finalize the message */
    genlmsg_end(msg, msg_head);

    /* send msg */
    genlmsg_multicast(&genl_test_gnl_family, msg, 0, group, GFP_ATOMIC);

    pr_info("genl_test_notify ok\n");
    return 0;

failure:
    kfree_skb(msg);
    return rc;
}

static void genl_test_timer_handler(struct timer_list *unused)
{
    genl_test_notify(GENL_TEST_MCG_0);
    genl_test_notify(GENL_TEST_MCG_1);

    mod_timer(&genl_test_timer, jiffies + msecs_to_jiffies(5000));
}

int __init init_module()
{
    int rc;

    rc = genl_register_family(&genl_test_gnl_family);
    if (rc)
        goto failure;
    
    timer_setup(&genl_test_timer, genl_test_timer_handler, 0);
    mod_timer(&genl_test_timer, jiffies + msecs_to_jiffies(5000));
        
    pr_info("genl_test init ok\n");
    return 0;

failure:
    pr_info("genl_test init failed\n");
    return rc;
}

void __exit cleanup_module()
{
    del_timer(&genl_test_timer);
    genl_unregister_family(&genl_test_gnl_family);

    pr_info("genl_test clearup ok\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wangrong1069@uniontech.com");
MODULE_DESCRIPTION("generic netlink demo");
