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

#生成CA证书请求
echo "-----   Creat CA Request    ------"
./miteCA gencsr -r "CN=SM2ROOTCA,C=CN,O=LZ,OU=HAHA,ST=ZHON" -o ${ca_dir}/ca
echo "----- Creat CA Request Done ------"
echo "Press to Continue..."
read
echo "----- SelfSign CA Request -----"
#自签名证书
./miteCA ca --new -c ${ca_dir}/ca.csr --key ${ca_dir}/ca.key --days 3650 --dir ${root_dir}

echo "----- SelfSign CA  Done ------"
echo "Press to Continue..."
read



#生成用户证书请求
echo "-----    Sign Client Cert      ------"
./miteCA gencsr -r "C=CN,CN=testClient,O=LZ,OU=HAHA,ST=ZHON" -o ${csr_dir}/sm2Client
cp ${csr_dir}/sm2Client.key ${key_dir}/sm2Client.key
#签发用户证书
./miteCA ca -c ${csr_dir}/sm2Client.csr --key sm2Client.key --days 365 --dir ${root_dir} -o sm2Client
echo "-----  Sign Client Cert  Done ------"
echo "Press to Continue..."
read

#生成服务器证书
echo "-----    Sign Server Cert      ------"
./miteCA gencsr -r "C=CN,CN=testServer,O=LZ,OU=HAHA,ST=ZHON" -o ${csr_dir}/sm2Server
cp ${csr_dir}/sm2Server.key ${key_dir}/sm2Server.key
#签发用户证书
./miteCA ca -c ${csr_dir}/sm2Server.csr --key sm2Server.key --days 365 --dir ${root_dir} -o sm2Server
echo "-----  Sign Server Cert  Done ------"
echo "Press to Continue..."
read

echo "-----------------------------------" >>${log_file}
