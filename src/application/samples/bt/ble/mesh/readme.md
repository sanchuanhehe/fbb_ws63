# ble mesh 编译相关:
# ble mesh相关选择Kconifg命令：./build.py ws63-liteos-mesh-sample-app menuconfig
# 将patch文件中内容同步到新的文件中: patch -p0 -r null < application/samples/bt/ble/mesh_adapter/patch/nimble_1_6_0.patch
# 将已经打入的patch回退(若不执行恢复会导致第二次同步时需要手动确认): patch -Rp0 -r null < application/samples/bt/ble/mesh_adapter/patch/nimble_1_6_0.patch
# ble mesh相关编译命令：./build.py ws63-liteos-mesh-sample-app