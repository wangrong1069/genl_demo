#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include "genl_test.h"



static int prep_nl_sock(struct nl_sock **nlsock)
{
    int family_id;

    *nlsock = nl_socket_alloc();
    if (!*nlsock) {
		fprintf(stderr, "nl_socket_alloc fail\n");
        return 1;
    }

    // nl_socket_disable_seq_check(*nlsock);
    // nl_socket_disable_auto_ack(*nlsock);

    /* connect to genl */
	if (genl_connect(*nlsock)) {
		fprintf(stderr, "genl_connect fail\n");
		goto exit_err;
	}

    /* resolve the generic nl family id*/
    family_id = genl_ctrl_resolve(*nlsock, GENL_TEST_FAMILY_NAME);
    if(family_id < 0){
		fprintf(stderr, "genl_ctrl_resolve fail\n");
		goto exit_err;
    }

    return 0;

exit_err:
    nl_socket_free(*nlsock);
    return 1;
}

static int send_msg_to_kernel(struct nl_sock *sock, char *str)
{
    struct nl_msg* msg;
    int family_id, err = 1;

    family_id = genl_ctrl_resolve(sock, GENL_TEST_FAMILY_NAME);
    if (family_id < 0) {
		fprintf(stderr, "genl_ctrl_resolve fail\n");
		return 1;
    }

    msg = nlmsg_alloc();
    if (!msg) {
		fprintf(stderr, "nlmsg_alloc fail\n");
		return 1;
    }

    if (!genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family_id, 0, 
        NLM_F_REQUEST, GENL_TEST_C_ECHO, 0)) {
		fprintf(stderr, "genlmsg_put fail\n");
		goto out;
    }

    if (nla_put_string(msg, GENL_TEST_A_MSG, str)) {
		fprintf(stderr, "nla_put_string fail\n");
		goto out;
    }

    if (nl_send_auto(sock, msg) < 0) {
		fprintf(stderr, "nla_put_string fail\n");
		goto out;
    }

    err = 0;
out:
    nlmsg_free(msg);
    return err;
}

static int print_msg(struct nl_msg *msg, void* arg)
{
    struct nlattr *attr[GENL_TEST_A_MAX+1];
    int ret = genlmsg_parse(nlmsg_hdr(msg), 0, attr, GENL_TEST_A_MAX, genl_test_genl_policy);

    if (ret < 0) {
		fprintf(stderr, "print_msg fail: %d\n", ret);
        return 0;
    }

    if (attr[GENL_TEST_A_MSG]) {
        printf("print_msg: %s\n", nla_get_string(attr[GENL_TEST_A_MSG]));
    }

    return 0;
}

static void handle_msg_from_kernel(struct nl_sock *sock, struct nl_cb *cb)
{
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, print_msg, NULL);

    nl_recvmsgs(sock, cb);
}

int main(int argc, char** argv)
{
	struct nl_sock* nlsock = NULL;
    struct nl_cb *cb = NULL;

    if (argc != 2) {
		fprintf(stderr, "%s \"STRING\"\n", argv[0]);
        return 1;
    }

	if (prep_nl_sock(&nlsock))
        return 1;

	if (send_msg_to_kernel(nlsock, argv[1]))
        return 1;
    
    cb = nl_cb_alloc(NL_CB_DEFAULT);
	handle_msg_from_kernel(nlsock, cb);

    nl_cb_put(cb);
    nl_socket_free(nlsock);
    return 0;
}
