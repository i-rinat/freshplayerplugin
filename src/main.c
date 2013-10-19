


int
main(void)
{

    const struct PPP_Instance_1_1 *ppp_instance_1_1 = ppp_get_interface(PPP_INSTANCE_INTERFACE_1_1);
    trace_info("p = %p\n", ppp_instance_1_1);
    trace_info("DidCreate = %p\n", ppp_instance_1_1->DidCreate);
    trace_info("DidDestroy = %p\n", ppp_instance_1_1->DidDestroy);
    trace_info("DidChangeView = %p\n", ppp_instance_1_1->DidChangeView);
    trace_info("DidChangeFocus = %p\n", ppp_instance_1_1->DidChangeFocus);
    trace_info("HandleDocumentLoad = %p\n", ppp_instance_1_1->HandleDocumentLoad);

    const char *argn[] = {"id"};
    const char *argv[] = {"hello"};
    ppp_instance_1_1->DidCreate(1, 1, argn, argv);


    return 0;
}
