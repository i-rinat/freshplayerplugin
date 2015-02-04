/*
 * Copyright © 2013-2015  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "main_thread.h"
#include "ppb_message_loop.h"
#include "pp_resource.h"
#include <pthread.h>
#include "trace.h"


static
void
wait_on_barrier(void *user_data, int32_t result)
{
    pthread_barrier_t *barrier = user_data;
    pthread_barrier_wait(barrier);
}

void *
fresh_wrapper_main_thread(void *p)
{
    struct pp_instance_s *pp_i = p;
    PP_Resource message_loop;

    message_loop = ppb_message_loop_create(pp_i->id);
    if (!message_loop) {
        trace_error("%s, can't create message loop\n", __func__);
        return NULL;
    }

    ppb_message_loop_attach_to_current_thread(message_loop);
    ppb_message_loop_proclaim_this_thread_main();

    // wait for barrier inside a task to ensure loop is running
    ppb_message_loop_post_work(message_loop,
                               PP_MakeCCB(wait_on_barrier, &pp_i->main_thread_barrier), 0);
    ppb_message_loop_run(message_loop);

    return NULL;
}
