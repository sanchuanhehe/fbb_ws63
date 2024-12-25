=========================================================
加密加签命令：./encryptandsign.sh 【参数1】 【参数2】 【参数3】

一、脚本参数说明：
【参数1】输入文件，待加密加签文件, 编译后生成，烧写包位于output/ws63/pktbin.zip，ota升级包位于output/ws63/upgrade/ota_pktbin.zip
【参数2】输出目录，加密加签后打包结果输出
【参数3】输入文件类型，all：烧写包，ota:升级包

=========================================================
二、sign_encry中目录或文件说明：
1)./ 加密加签工具所在目录，所有的加密加签工具、程序、脚本、都在此目录；
tool_security：    flash秘钥派生工具
sign_tool_pltuni： 签名加密工具
2)./config/sign，签名、flash加密配置文件和秘钥，其中：
ec_bp256_oem_root_public_key.pem 为根公钥，用于安全启动特性，生成hash值在root_pubk.bin.hash中
ec_bp256_oem_root_private_key.pem为根私钥，用于安全启动特性镜像签名flashboot.bin
其它为多级秘钥
3)./config/ota，fota升级包头配置文件和工具秘钥配置路径

=========================================================
三、一键随机生成所有秘钥 (慎用，生成后会覆盖上次所有秘钥)
1.生成秘钥命令
cd ./sign_encry
./gen_all_key.sh
