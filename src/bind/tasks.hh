/**
 ** \file bind/tasks.hh
 ** \brief Bind module related tasks.
 */

 #pragma once

 #include <task/libtask.hh>
 
 namespace bind::tasks
 {
   TASK_GROUP("bind");
 
   TASK_DECLARE("bindings-compute",
                "compute identifier bindings",
                bindings_compute,
                "parse");
 } // namespace bind::tasks