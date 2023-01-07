#!/bin/bash
cd `dirname $0`
proj_name="bg"
rm -rf ${proj_name} || true
vivado_hls -f script.tcl
if [ $? != 0 ]; then
    echo "Failed to export hdl." 1>&2
    exit 1
fi
current_dir=`pwd`
cp ${proj_name}/solution1/syn/report/bg_csynth.rpt .
echo "Report is saved in ${current_dir}/bg_csynth.rpt."
rm -rf ip || true
cp -r ${proj_name}/solution1/impl/ip .
echo "IP is saved in ${current_dir}/ip."
