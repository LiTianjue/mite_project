#!/bin/bash

root_dir=demoCA
ca_dir=${root_dir}/ca
cert_dir=${root_dir}/certs
key_dir=${root_dir}/private
csr_dir=${root_dir}/csrs
crl_dir=${root_dir}/crls

log_file=log.txt

current_time=`date`
echo "" >> ${log_file} 
echo "-----------------------------------" >>${log_file}
echo ${current_time} >> ${log_file}
mkdir ${root_dir}		2>>${log_file}
mkdir ${ca_dir}			2>>${log_file}
mkdir ${cert_dir}		2>>${log_file}
mkdir ${key_dir}		2>>${log_file}
mkdir ${csr_dir}		2>>${log_file}
mkdir ${crl_dir}		2>>${log_file}

#生成服务器证书
echo "-----    Sign Server Cert      ------"
./miteCA gencsr -r "CN=sm2Server_Sign,O=LZ,OU=HAHA,ST=ZHON" -o ${csr_dir}/sm2ServerSign
cp ${csr_dir}/sm2ServerSign.key ${key_dir}/sm2ServerSign.key
#签发用户证书
./miteCA ca -c ${csr_dir}/sm2ServerSign.csr --key sm2Server.key --days 365 --dir ${root_dir} -o sm2ServerSign
echo "-----  Sign Server Cert  Done ------"
echo "Press to Continue..."
read

echo "-----------------------------------" >>${log_file}
