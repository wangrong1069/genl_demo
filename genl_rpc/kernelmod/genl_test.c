#include <linux/kernel.h>
#include <linux/module.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include <linux/skbuff.h>

#include "../genl_test.h"

static struct genl_family genl_test_gnl_family;

static int genl_test_echo(struct sk_buff *skb, struct genl_info *info)
{
    /* message handling code goes here; return 0 on success, negative
        * values on failure */
    
    int rc;
    void *msg_head;
    char buf[1024];
    struct sk_buff *msg;

    /* alloc  */
    msg = genlmsg_new(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!msg)
		return -ENOMEM;

    /* construct msg */
    /* create the message headers */
    msg_head = genlmsg_put_reply(msg, info, &genl_test_gnl_family, GFP_ATOMIC, GENL_TEST_C_ECHO);
    if (!msg_head) {
        rc = -ENOMEM;
        goto failure;
    }
    /* add a GENL_TEST_A_MSG attribute */
    buf[0] = 0;
    strcat(buf, "Generic Netlink Echo: ");
    if (info->attrs[GENL_TEST_A_MSG]) {
        strncat(buf, (char*)nla_data(info->attrs[GENL_TEST_A_MSG]), sizeof(buf)-strlen(buf)-1);
    }
    rc = nla_put_string(msg, GENL_TEST_A_MSG, buf);
    if (rc != 0)
        goto failure;
    /* finalize the message */
    genlmsg_end(msg, msg_head);

    /* send msg */
    genlmsg_reply(msg, info);

    pr_info("genl_test_echo ok\n");
    return 0;
failure:
    kfree_skb(msg);
    return rc;
}

/* operation definition */
static struct genl_ops genl_test_gnl_ops[] = {
    {
        .cmd = GENL_TEST_C_ECHO,
        .policy = genl_test_genl_policy,
        .doit = genl_test_echo,
        .dumpit = NULL,
    }
};

/* family definition */
static struct genl_family genl_test_gnl_family = {
    .name = GENL_TEST_FAMILY_NAME,
    .version = 1,
    .module = THIS_MODULE,
    .maxattr = GENL_TEST_A_MAX,
    .netnsok = false,
    .ops = genl_test_gnl_ops,
    .n_ops = ARRAY_SIZE(genl_test_gnl_ops),
};


int __init init_module()
{
    int rc;

    rc = genl_register_family(&genl_test_gnl_family);
    if (rc)
        goto failure;
        
    pr_info("genl_test init ok\n");
    return 0;
failure:
    pr_info("genl_test init failed\n");
    return rc;
}

void __exit cleanup_module()
{
    genl_unregister_family(&genl_test_gnl_family);

    pr_info("genl_test clearup ok\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wangrong1069");
MODULE_DESCRIPTION("generic netlink demo");
