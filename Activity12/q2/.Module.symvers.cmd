cmd_/home/kuranasaki/work/2110313-OS-SYS-PROG/Activity12/q2/Module.symvers := sed 's/\.ko$$/\.o/' /home/kuranasaki/work/2110313-OS-SYS-PROG/Activity12/q2/modules.order | scripts/mod/modpost -m -a  -o /home/kuranasaki/work/2110313-OS-SYS-PROG/Activity12/q2/Module.symvers -e -i Module.symvers   -T -
