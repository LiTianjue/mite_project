//test arguments
//ca --new -d d:/testca/ -c d:/test_csr.csr --key d:/test_csr.key -v


#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include <cstring>
#include <sys/stat.h>
#include <error.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <openssl/safestack.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

using namespace std;
#define prt(x) cout << x << endl
#define err(x) cerr << x << endl
#define vb(x)  if (ca_verbose) prt(x)

#define DEFAULT_MODE	0777

void ca_print_help()
{
    err("Usage:");
    err("--new              : Setup new CA");
    err("--sign             : Sign CSR");
    err("--dir, -d <dir>    : CA directory(must specify)");
    err("--csr, -c <path>   : CSR file");
    err("--key <path>       : Private key(only needed when setting up new CA)");
    err("-o <path>			: OutPut file prefix");
    err("--days <number>    : Certificate valid time since now");
    err("--key_usage        : Specify key usage");
    err("--ext_key_usage    : Specify extended key sage");
    err("--help, -h         : Print this message");
    err("--verbose, -v      : Show verbose");
}

bool ca_verbose = false;
int ca_mode = 0;
string ca_dir;
string _csr_file;
string _csr_key_file;
string _key_usage;
string _ext_key_usage;
long days = 365;
//string prv_file;
string cert_file;


int newca()
{
    vb("Creating directories");
	//vb(ca_dir);
    if (mkdir(ca_dir.c_str(),DEFAULT_MODE)) {
        //err("CA directory invalid or already exists");
        //return 3;
    }
    if (mkdir((ca_dir + "/certs").c_str(),DEFAULT_MODE)) {
        //err("CA directory invalid or already exists");
        //return 3;
    }
    if (mkdir((ca_dir + "/crls").c_str(),DEFAULT_MODE)) {
        //err("CA directory invalid or already exists");
        //return 3;
    }
    if (mkdir((ca_dir + "/private").c_str(),DEFAULT_MODE)) {
        //err("CA directory invalid or already exists");
        //return 3;
    }
    vb("Creating files");
    FILE *f;
    f = fopen((ca_dir + "/index.txt").c_str(), "w");
    fclose(f);
    f = fopen((ca_dir + "/serial").c_str(), "w");
    fwrite("1", 1, 2, f);
    fclose(f);
    
    EVP_PKEY *pk;
	vb(_csr_key_file); 
    f = fopen(_csr_key_file.c_str(), "r");
	if(f== NULL)
	{
		perror("OPEN:");
	}
    pk = PEM_read_PrivateKey(f, 0, 0, 0);
    if (!pk) {
        err("Failed to read private key");
    }
    fclose(f);
    
	//vb(_csr_file); 
    f = fopen(_csr_file.c_str(), "r");
    X509_REQ *req = PEM_read_X509_REQ(f, 0, 0, 0);
    if (1 != X509_REQ_verify(req, pk)) {
        err("Private key does not match");
        return 8;
    }
    fclose(f);
    X509 *x = X509_new();
	//设置版本号
    X509_set_version(x, 2);
	//序列号
    ASN1_INTEGER_set(X509_get_serialNumber(x), 0);
	//填充信息
    X509_set_issuer_name(x,  X509_REQ_get_subject_name(req));
    X509_set_subject_name(x, X509_REQ_get_subject_name(req));
	//有效期
    X509_gmtime_adj(X509_get_notBefore(x), 0);
    X509_gmtime_adj(X509_get_notAfter(x), (long)60 * 60 * 24 * days);
	//放入公钥
    X509_set_pubkey(x, pk);
	//扩展信息
    X509_EXTENSION *ex;
    ex = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, (char*)"critical,CA:TRUE");
    X509_add_ext(x, ex, -1);
    X509_EXTENSION_free(ex);
    ex = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, (char*)"cRLSign,keyCertSign");
    X509_add_ext(x, ex, -1);
    X509_EXTENSION_free(ex);
    ex = X509V3_EXT_conf_nid(NULL, NULL, NID_ext_key_usage, (char*)"clientAuth,emailProtection,OCSPSigning,timeStamping,codeSigning,serverAuth,ipsecEndSystem,ipsecTunnel,ipsecUser,ipsecEndSystem");
    X509_add_ext(x, ex, -1);
    X509_EXTENSION_free(ex);
	//自签名 
    //X509_sign(x, pk, EVP_sha384());
    X509_sign(x, pk, EVP_sm3());
    if (ca_verbose) {
        X509_print_fp(stdout, x);
    }
	//写入文件
    f = fopen((ca_dir + "/ca.crt").c_str(), "wb");
    //if (!PEM_write_X509_AUX(f, x)) {
    if (!PEM_write_X509(f, x)) {
        err("Failed");
        return 8;
    }
	fclose(f);

    f = fopen((ca_dir + "/ca/ca.crt").c_str(), "wb");
    //if (!PEM_write_X509_AUX(f, x)) {
    if (!PEM_write_X509(f, x)) {
        err("Failed");
        //return 8;
    }
	fclose(f);

    f = fopen((ca_dir + "/private/ca.key").c_str(), "wb");
    //if (!PEM_write_PrivateKey(f, pk, EVP_aes_256_cbc(), 0, 0, 0, 0)) {
    if (!PEM_write_PrivateKey(f, pk,NULL, 0, 0, 0, 0)) {
        err("Failed");
        return 8;
    }
    fclose(f);
    vb("Done");
}

int casign()
{
#if 1
	vb("-----> Sign a user cert");
	FILE *f;

	EVP_PKEY *cpk;		//CA的密钥
	EVP_PKEY *pk;		//用户的公钥

	//REQ
	vb("Open csr :" + _csr_file);
	f = fopen(_csr_file.c_str(),"r");
	if(!f)
	{
		perror("open");
	}
	X509_REQ *req = PEM_read_X509_REQ(f,0,0,0);
	if(!req)
	{
		printf("Read REQ file fail.\n");
	}
	//Public 
	pk = X509_REQ_get_pubkey(req);

	if(1 != X509_REQ_verify(req,pk)) {
		err("Private key does not match");
		return 8;
	}
	fclose(f);

	//读取CA证书
	vb(ca_dir + "/ca.crt");
    f = fopen((ca_dir + "/ca/ca.crt").c_str(), "r");
	if(f == NULL)
		perror("read Ca Cert");
	//X509 *x_ca = PEM_read_X509(f,0,0,0);
	X509 *x_ca = PEM_read_X509_AUX(f,0,0,0);
	if(!x_ca)
	{
		err("Faile to Read CA Cert.");
	}
	fclose(f);

	// 读取CA的私钥
	f = fopen((ca_dir+"/ca/ca.key").c_str(),"r");
	cpk = PEM_read_PrivateKey(f,0,0,0);
	if(!pk)
	{
		err("Failed to read CA privete key");
	}
	fclose(f);


    X509 *x = X509_new();
	//设置版本号
    X509_set_version(x, 2);
	//序列号
    ASN1_INTEGER_set(X509_get_serialNumber(x), 0);
	//填充签发者(CA)信息
    //X509_set_issuer_name(x,  X509_REQ_get_subject_name(req));
    X509_set_issuer_name(x,  X509_get_subject_name(x_ca));
	//填充用户信息
    X509_set_subject_name(x, X509_REQ_get_subject_name(req));
	//有效期
    X509_gmtime_adj(X509_get_notBefore(x), 0);
    X509_gmtime_adj(X509_get_notAfter(x), (long)60 * 60 * 24 * days);
	//放入公钥
    X509_set_pubkey(x, pk);
	//扩展信息
    X509_EXTENSION *ex;
	/* 用户证书不要这个项目
    ex = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, (char*)"critical,CA:FALSE");
    X509_add_ext(x, ex, -1);
    X509_EXTENSION_free(ex);
	*/

	//测试的时候不填充密钥用法
# if 0
    ex = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, (char*)"cRLSign,keyCertSign");
    X509_add_ext(x, ex, -1);
    X509_EXTENSION_free(ex);
# endif
	
    ex = X509V3_EXT_conf_nid(NULL, NULL, NID_ext_key_usage, (char*)"clientAuth,emailProtection,OCSPSigning,timeStamping,codeSigning,serverAuth,ipsecEndSystem,ipsecTunnel,ipsecUser,ipsecEndSystem");
    X509_add_ext(x, ex, -1);
    X509_EXTENSION_free(ex);
    
	//X509_sign(x, pk, EVP_sha384());
	//使用CA的私钥签名证书
    X509_sign(x, cpk, EVP_sm3());
    if (ca_verbose) {
        X509_print_fp(stdout, x);
    }

	//写入文件暂时写死
    f = fopen((ca_dir + "/certs/" + cert_file).c_str(), "wb");
    //if (!PEM_write_X509_AUX(f, x)) {
    if (!PEM_write_X509(f, x)) {
        err("Failed");
        return 8;
    }
	fclose(f);
	/*
    f = fopen((ca_dir + "/private/" + prv_file).c_str(), "wb");
    if (!PEM_write_PrivateKey(f, pk, EVP_aes_256_cbc(), 0, 0, 0, 0)) {
        err("Failed");
        return 8;
    }
	*/
    vb("Done");
#endif
}

int main_ca(int argc, char* argv[])
{   
    if (argc < 2) {
        ca_print_help();
        return 1;
    }
    
    const char *short_opts = "c:o:d:v";
    struct option long_opts[] = {
    {"dir", required_argument, NULL, 'd'},
    {"new", no_argument, NULL, 'n'},
    {"csr", required_argument, NULL, 'c'},
    {"days", required_argument, NULL, 271},
    {"out", required_argument, NULL, 'o'},
    {"key", required_argument, NULL, 'k'},
    {"key_usage", required_argument, NULL, 272},
    {"ext_key_usage", required_argument, NULL, 273},
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    };
    int c;
    while ((c = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (c) {
        case 'n':
            ca_mode = 1;
            break;
        case 'd':
            ca_dir = optarg;
            break;
        case 'c':
            _csr_file = optarg;
            break;
        case 271:
            days = strtol(optarg, 0, 10);
            break;
        case 272:
            _key_usage = optarg;
            break;
        case 273:
            _ext_key_usage = optarg;
            break;
        case 'k':
            _csr_key_file = optarg;
            break;
        case 'h':
            ca_print_help();
            return 0;
            break;
        case 'v':
            ca_verbose = true;
            break;
		case 'o':
			//prv_file = string(optarg) + ".key";
			cert_file = string(optarg) + ".crt";
			break;
        default:
            ca_print_help();
            return 1;
            break;
        }
    }
    if (ca_dir.empty()) {
        err("Must specify CA diractory");
        ca_print_help();
        return 1;
    }
    if (_csr_file.empty()) {
        err("Must specify a CSR file");
        return 2;
    }
    if (ca_mode == 1) {
        if (_csr_key_file.empty()) {
            err("Must specify the private key");
            return 2;
        }
        return newca();
    } else {
		// really do what we going to do
		casign();
	}
    
    
    return 0;
}


