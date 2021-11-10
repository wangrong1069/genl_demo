#ifndef GENL_TEST_H
#define GENL_TEST_H

/* protocol family */
#define GENL_TEST_FAMILY_NAME "genl_test"

/* attributes */
enum {
    GENL_TEST_A_UNSPEC,
    GENL_TEST_A_MSG,
    __GENL_TEST_A_MAX,
};
#define GENL_TEST_A_MAX (__GENL_TEST_A_MAX - 1)

/* attribute policy */
static struct nla_policy genl_test_genl_policy[GENL_TEST_A_MAX + 1] = {
#ifdef __KERNEL__
    [GENL_TEST_A_MSG] = { .type = NLA_NUL_STRING, .len = 2048 },
#else
    [GENL_TEST_A_MSG] = { .type = NLA_NUL_STRING, .maxlen = 2048 },
#endif
};

/* commands */
enum {
    GENL_TEST_C_UNSPEC,
    GENL_TEST_C_ECHO,
    __GENL_TEST_C_MAX,
};
#define GENL_TEST_C_MAX (__GENL_TEST_C_MAX - 1)

#endif