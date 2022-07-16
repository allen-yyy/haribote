
void init_all_device();

typedef int (*initfunc) (void);
extern initfunc __initfunc_start,__initfunc_end;
#define __init_func __attribute__ ((unused,__section__ (".initfunc.init")))
#define __initfunc(f) static initfunc __initfunc_##f __init_func = f
