#define NOMINMAX
#include <cstdint>
#include <chrono>
#include <ctime>
#include <pthread.h>
#include <stdlib.h>
#include "UCPClient.h"
#include <fcntl.h>
//#include <stddef.h>

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#include <Windows.h>
#include <VersionHelpers.h>
#include <io.h>
#include <BaseTsd.h>
#endif

#ifdef __linux__
#include <sys/socket.h> 
#include <netdb.h>
#include "_kernel.h"
#endif

#include <ctime>
#include "Log.h"
#include <sstream>
#include "Constants.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define MAX_GPUS 16
#define DEFAULT_BLOCKSIZE 0x2000
#define DEFAULT_THREADS_PER_BLOCK 256

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"
// #define MODEMDEVICE "/dev/ttyUSB1"
#define _POSIX_SOURCE 1         //POSIX compliant source

	int fd = -1; /* Файловый дескриптор для порта */


#define ROTR64(x, n)  (((x) >> (n)) | ((x) << (64 - (n))))

#define B2B_G(v,a,b,c,d,x,y,c1,c2) { \
	v[a] += v[b] + (x ^ c1); \
	v[d] ^= v[a]; \
	v[d] = ROTR64(v[d], 60); \
	v[c] += v[d]; \
	v[b] = ROTR64(v[b] ^ v[c], 43); \
	v[a] +=  v[b] + (y ^ c2); \
	v[d] = ROTR64(v[d] ^ v[a], 5); \
	v[c] +=  v[d]; \
	v[b] = ROTR64(v[b] ^ v[c], 18); \
	v[d] ^= (~v[a] & ~v[b] & ~v[c]) | (~v[a] & v[b] & v[c]) | (v[a] & ~v[b] & v[c])   | (v[a] & v[b] & ~v[c]); \
    v[d] ^= (~v[a] & ~v[b] & v[c]) | (~v[a] & v[b] & ~v[c]) | (v[a] & ~v[b] & ~v[c]) | (v[a] & v[b] & v[c]); \
}

static const uint8_t sigma[16][16] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 },
	{ 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 },
	{ 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 },
	{ 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 },
	{ 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 },

	{ 12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11 },
	{ 13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10 },
	{ 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5 },
	{ 10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13 , 0 },

	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 },
	{ 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 },
	{ 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 },
	{ 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 },
	{ 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 }
};

static const uint64_t u512[16] =
{
	0xA51B6A89D489E800UL, 0xD35B2E0E0B723800UL,
	0xA47B39A2AE9F9000UL, 0x0C0EFA33E77E6488UL,
	0x4F452FEC309911EBUL, 0x3CFCC66F74E1022CUL,
	0x4606AD364DC879DDUL, 0xBBA055B53D47C800UL,
	0x531655D90C59EB1BUL, 0xD1A00BA6DAE5B800UL,
	0x2FE452DA9632463EUL, 0x98A7B5496226F800UL,
	0xBAFCD004F92CA000UL, 0x64A39957839525E7UL,
	0xD859E6F081AAE000UL, 0x63D980597B560E6BUL
};

static const uint64_t vBlake_iv[8] = {
	0x4BBF42C1F006AD9Dul, 0x5D11A8C3B5AEB12Eul,
	0xA64AB78DC2774652ul, 0xC67595724658F253ul,
	0xB8864E79CB891E56ul, 0x12ED593E29FB41A1ul,
	0xB1DA3AB63C60BAA8ul, 0x6D20E50C1F954DEDul
};


void vblake512_compress(unsigned long *h, unsigned long *mc)
{
	unsigned long v[16];
	unsigned long m[16] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //#pragma unroll 8
	for (int i = 0; i < 8; i++)
	m[i] = mc[i];
	//#pragma unroll 8
	for (int i = 0; i < 8; i++) {
		v[i] = h[i];
		v[i + 8] = vBlake_iv[i];
	}
	v[12] ^= 64;
	v[14] ^= (ulong)(0xfffffffffffffffful);// (long)(-1);


	#pragma unroll 16
	
	for (int i = 0; i < 16; i++) {
		B2B_G(v, 0, 4, 8, 12, m[sigma[i][1]], m[sigma[i][0]],
			u512[sigma[i][1]], u512[sigma[i][0]]);

		B2B_G(v, 1, 5, 9, 13, m[sigma[i][3]], m[sigma[i][2]],
			u512[sigma[i][3]], u512[sigma[i][2]]);

		B2B_G(v, 2, 6, 10, 14, m[sigma[i][5]], m[sigma[i][4]],
			u512[sigma[i][5]], u512[sigma[i][4]]);

		B2B_G(v, 3, 7, 11, 15, m[sigma[i][7]], m[sigma[i][6]],
			u512[sigma[i][7]], u512[sigma[i][6]]);

		B2B_G(v, 0, 5, 10, 15, m[sigma[i][9]], m[sigma[i][8]],
			u512[sigma[i][9]], u512[sigma[i][8]]);

		B2B_G(v, 1, 6, 11, 12, m[sigma[i][11]], m[sigma[i][10]],
			u512[sigma[i][11]], u512[sigma[i][10]]);

		B2B_G(v, 2, 7, 8, 13, m[sigma[i][13]], m[sigma[i][12]],
			u512[sigma[i][13]], u512[sigma[i][12]]);

		B2B_G(v, 3, 4, 9, 14, m[sigma[i][15]], m[sigma[i][14]],
			u512[sigma[i][15]], u512[sigma[i][14]]);
	}

	h[0] ^= v[0] ^ v[8];

	h[3] ^= v[3] ^ v[11];

	h[6] ^= v[6] ^ v[14];


	h[0] ^= h[3] ^ h[6];  //copied from  the java

}

unsigned long vBlake2( uint64_t *hi, uint64_t h7)
{
	unsigned long b[8];
	unsigned long h[8];
  // #pragma unroll 8
	for (int i = 0; i < 8; i++) {
		h[i] = vBlake_iv[i];
		b[i] = hi[i];
	}
	h[0] ^= (ulong)(0x01010000 ^ 0x18);

	b[7] = h7;

	vblake512_compress(h, b);

	return h[0];
}




int openserial(char *devicename)
{
	char	strbuf[256];
  	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
		sprintf(strbuf, "open_port: Unable to open %s\n", devicename);
		perror(strbuf);
	}

	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &options);

    return fd;
}






pthread_mutex_t stratum_sock_lock;
pthread_mutex_t stratum_log_lock;
//cl_device_id *devices; //TODOS
int blocksize = DEFAULT_BLOCKSIZE;
int threadsPerBlock = DEFAULT_THREADS_PER_BLOCK;
int opt_platform_id = -1;
bool verboseOutput = false;
int amd_flag = 0;

//#define open _open
const char *source = NULL;
size_t source_len;
char *binary = NULL;
size_t binary_len;
uint32_t lastNonceStart = 0;
char outputBuffer[8192];
char selectedDeviceName[100];

//amd stuff

UCPClient* pUCP;

struct mining_attr {
	int dev_id;

};

int opt_n_threads = 1;
short device_map[MAX_GPUS] = { 0 };
int gpu_threads = 1;
int active_gpus;
char device_name[MAX_GPUS][256];
long  device_sm[MAX_GPUS] = { 0 };
short device_mpcount[MAX_GPUS] = { 0 };
int init[MAX_GPUS] = { 0 };
uint32_t lastnonce[4] = { 6,7,8,9 };



void promptExit(int exitCode)
{
	cout << "Exiting in 10 seconds..." << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	exit(exitCode);
}

/**
* Takes the provided timestamp and places it in the header
*/
void embedTimestampInHeader(uint8_t *header, uint32_t timestamp)
{
	header[55] = (timestamp & 0x000000FF);
	header[54] = (timestamp & 0x0000FF00) >> 8;
	header[53] = (timestamp & 0x00FF0000) >> 16;
	header[52] = (timestamp & 0xFF000000) >> 24;
}

/**
* Returns a 64-byte header to attempt to mine with.
*/
void getWork(UCPClient& ucpClient, uint32_t timestamp, uint64_t *header)
{
	//uint64_t *header = new uint64_t[8];
	//printf("time stamp %08x \n", timestamp);
	ucpClient.copyHeaderToHash((byte *)header);
	embedTimestampInHeader((uint8_t*)header, timestamp);
	//return header;
}

int deviceToUse = 0;

#if NVML
nvmlDevice_t device;
void readyNVML(int deviceIndex) {
	nvmlInit();
	nvmlDeviceGetHandleByIndex(deviceIndex, &device);
}
int getTemperature() {
	unsigned int temperature;
	nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temperature);
	return temperature;
}

int getCoreClock() {
	unsigned int clock;
	nvmlDeviceGetClock(device, NVML_CLOCK_GRAPHICS, NVML_CLOCK_ID_CURRENT, &clock);
	return clock;
}

int getMemoryClock() {
	unsigned int memClock;
	nvmlDeviceGetClock(device, NVML_CLOCK_MEM, NVML_CLOCK_ID_CURRENT, &memClock);
	return memClock;
}
#else
void readyNVML(int deviceIndex) {
	// Do Nothing
}

int getTemperature() {
	return -1;
}

int getCoreClock() {
	return -1;
}

int getMemoryClock() {
	return -1;
}
#endif

#define SHARE_SUBMISSION_NO_RESPONSE_WARN_THRESHOLD 50

void vprintf(char* toprint) {
	if (verboseOutput) {
		printf(toprint);
	}
}

void printHelpAndExit() {
	printf("VeriBlock vBlake OpenCL Miner v1.0\n");
	printf("Required Arguments:\n");
	printf("-o <poolAddress>           The pool address to mine to in the format host:port\n");
	printf("-u <username>              The username (often an address) used at the pool\n");
	printf("Optional Arguments:\n");
	printf("-p <password>              The miner/worker password to use on the pool\n");
	printf("-d <deviceNum>             The ordinal of the device to use (default 0)\n");
	printf("-tpb <threadPerBlock>      The threads per block to use with the Blake kernel (default %d)\n", DEFAULT_THREADS_PER_BLOCK);
	printf("-bs <blockSize>            The blocksize to use with the vBlake kernel (default %d)\n", DEFAULT_BLOCKSIZE);
	printf("-l <enableLogging>         Whether to log to a file (default true)\n");
	printf("-v <enableVerboseOutput>   Whether to enable verbose output for debugging (default false)\n");
	printf("\n");
	printf("Example command line:\n");
	printf("VeriBlock-NodeCore-PoW-CUDA -u VHT36jJyoVFN7ap5Gu77Crua2BMv5j -o testnet-pool-gpu.veriblock.org:8501 -l false\n");
	promptExit(0);
}

#ifdef _WIN32
static WSADATA g_wsa_data;
#endif

char net_init(void)
{
#ifdef _WIN32
	return (WSAStartup(MAKEWORD(2, 2), &g_wsa_data) == NO_ERROR);
#elif __linux__
	return 1;
#endif
}

void net_deinit(void)
{
#ifdef _WIN32
	WSACleanup();
#endif
}

string net_dns_resolve(const char* hostname)
{
	struct addrinfo hints, *results, *item;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  /* AF_INET6 to force version */
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(hostname, NULL, &hints, &results)) != 0)
	{
		fprintf(stderr, "failed to resolve hostname \"%s\": %s", hostname, gai_strerror(status));
		return "invalid hostname";
	}

	printf("IP addresses for %s:\n\n", hostname);

	string ret;

	for (item = results; item != NULL; item = item->ai_next)
	{
		void* addr;
		char* ipver;

		/* get pointer to the address itself */
		/* different fields in IPv4 and IPv6 */
		if (item->ai_family == AF_INET)  /* address is IPv4 */
		{
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)item->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else  /* address is IPv6 */
		{
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)item->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		/* convert IP to a string and print it */
		inet_ntop(item->ai_family, addr, ipstr, sizeof ipstr);
		printf("  %s: %s\n", ipver, ipstr);
		ret = ipstr;
	}

	freeaddrinfo(results);
	return ret;
}


void dump(const char *fname, void *data, size_t len)
{
	int			fd;
	ssize_t		ret;
	if (-1 == (fd = open(fname, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, 0666))) {
		sprintf(outputBuffer, "%s: %s", fname, strerror(errno));
		cout << outputBuffer << endl;
		Log::error(outputBuffer);
		promptExit(-1);

	}

	ret = write(fd, data, len);
	if (ret == -1) {
		sprintf(outputBuffer, "write: %s: %s", fname, strerror(errno));
		cout << outputBuffer << endl;
		Log::error(outputBuffer);
		promptExit(-1);

	}

	if ((size_t)ret != len) {
		sprintf(outputBuffer, "%s: partial write", fname, strerror(errno));
		cout << outputBuffer << endl;
		Log::error(outputBuffer);
		promptExit(-1);

	}

	if (-1 == close(fd)) {
		sprintf(outputBuffer, "close: %s: %s", fname, strerror(errno));
		cout << outputBuffer << endl;
		Log::error(outputBuffer);
		promptExit(-1);

	}

}



void kernel_vblake(uint *nonceStart, uint *nonceOut, unsigned long *hashStartOut, unsigned long  *headerIn)
{
	// Generate a unique starting nonce for each thread that doesn't overlap with the work of any other thread
	uint nonce = 0;//nonceStart[0];//((uint)get_global_id(0)&0xffffffffu) + 
uint n = 0;
	do {
nonce = n;
	unsigned long nonceHeaderSection = headerIn[7];

	
		nonceHeaderSection &= 0x00000000FFFFFFFFu;
		nonceHeaderSection |= (((unsigned long)nonce) << 32);

		unsigned long hashStart = vBlake2(headerIn, nonceHeaderSection);

 //printf("H=  0x%jx)\n",  hashStart);	

//sleep(1000);
		if ((hashStart & 0x00000000FFFFFFFFu) == 0) { // 2^32 difficulty
								
			//if (hashStartOut[0] > hashStart || hashStartOut[0] == 0) {
				nonceOut[0] = nonce;
				hashStartOut[0] = hashStart;
				printf("HASH=  0x%jx)\n",  hashStart);	
				return 1;
			//}
			
		}
		n++;
		} while (n < 0x0000000000ffFFFF );	
		
		
}





uint32_t recvserial(int fd)
{
	uint32_t goodnonce = 0;
	uint32_t temperature;
 	int res;
	unsigned char In1;
	uint32_t gnonce;//uint64_t
	unsigned char buf[4];//4
	int gnonceBytes =0 ;
	int good=0;
				res = read(fd,buf,4);//4
				if (res>0)
				{
					for (int i=0; i<res; i++)  //for all chars in string
					{
						In1 = buf[i];
						//fprintf(stdout,"[%02x] ",In1);
						//fflush(stdout);

						gnonce <<= 8;
						gnonce |= In1;
						//fprintf(stdout,"gnonce  [%#010x] ",gnonceBytes);
						//fflush(stdout);	
						if (++gnonceBytes == 4)
						{
							good = 1;
							gnonceBytes = 0;
						}

					} // end of for all chars in string
				}  // end if res>0

		if (good==1) 
		{

  		goodnonce 	= gnonce;// >> 32;

		return goodnonce;
		} else {
		return 0;
		}	
}

void unpack_uint64(uint64_t number, uint8_t *result) {
/*
    result[0] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[1] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[2] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[3] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[4] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[5] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[6] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[7] = number & 0x00000000000000FF ;
*/
    result[7] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[6] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[5] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[4] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[3] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[2] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[1] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[0] = number & 0x00000000000000FF ;

}



void* miner_thread(void* arg) {
	struct mining_attr *arg_Struct =
		(struct mining_attr*) arg;
	short thr_id = arg_Struct->dev_id;
	uint32_t end_nonce = 0x20000000u * (thr_id + 1);
	// Run initialization of device before beginning timer
	uint64_t header[8];
	pthread_mutex_lock(&stratum_sock_lock);
	getWork(*pUCP, (uint32_t)std::time(0), header);
	pthread_mutex_unlock(&stratum_sock_lock);
	pthread_mutex_lock(&stratum_log_lock);
	unsigned long long startTime = std::time(0);
	pthread_mutex_unlock(&stratum_log_lock);

	uint32_t nonceResult[1] = { 0 };
	uint64_t hashStart[1] = { 0 };
	uint32_t startNonce = 0x20000000u * thr_id;
	uint32_t lastNonce1 = startNonce;
	char line[256];

	size_t		global_ws;
	unsigned long long hashes = 0;
	size_t  local_work_size = (unsigned int)threadsPerBlock;
	uint64_t  *phashstartout, *pheaderin;
	uint32_t  *pnoncestart, *pnonceout;
//TODOS	cl_mem   pnoncestart_d, pnonceout_d, phashstartout_d, pheaderin_d;
	//cudaError_t cudaStatus;  replaced with status
	pnoncestart = (uint32_t*)malloc(sizeof(uint32_t) * 2);
	pnonceout = (uint32_t *)malloc(sizeof(uint32_t) * 2);
	phashstartout = (uint64_t *)malloc(sizeof(uint64_t) * 2);
	pheaderin = (uint64_t *)malloc(sizeof(uint64_t) * 9);

//TODOS	pnoncestart_d = check_clCreateBuffer(context[thr_id], CL_MEM_READ_WRITE, sizeof(uint32_t) * 1, NULL);
//TODOS	pnonceout_d = check_clCreateBuffer(context[thr_id], CL_MEM_READ_WRITE, sizeof(uint32_t) * 1, NULL);
//TODOS	phashstartout_d = check_clCreateBuffer(context[thr_id], CL_MEM_READ_WRITE, sizeof(uint64_t) * 1, NULL);
//TODOS	pheaderin_d = check_clCreateBuffer(context[thr_id], CL_MEM_READ_WRITE, sizeof(uint64_t) * 8, NULL);

	uint32_t count = 0;

	int numLines = 0;
	printf("got in thread innit\n");
	// Mining loop
	while (true) {
		vprintf("top of mining loop\n");
		count++;
		long timestamp = (long)std::time(0);
		//delete[] header;
		vprintf("Getting work...\n");
		pthread_mutex_lock(&stratum_sock_lock);
		getWork(*pUCP, timestamp, header);
		vprintf("Getting job id...\n");
		int jobId = pUCP->getJobId();
		pthread_mutex_unlock(&stratum_sock_lock);
		count++;
		vprintf("Running kernel...\n");
		//printf("H=  0x%jx)\n",  header[7]);	

		/*
		
  header[0] = 0x798f0200b0540000ULL;
  header[1] = 0xf88940e77681cbbdULL;
  header[2] = 0x43b67ea964231229ULL; 
  header[3] = 0xf2fe6082ba634ed4ULL;
  header[4] = 0x7b7d9eb035f86bd2ULL; 
  header[5] = 0x205aafeabd6ac03bULL;
  header[6] = 0x3fc4a45c36e4126cULL; 
  header[7] = 0x0000000006d40207ULL;
//0x00b006a5
//0x00b00624			
	*/	for (int i = 0; i<8; i++){
			pheaderin[i] = header[i];
		//printf("H[%d]=  0x%jx)\n", i, header[i]);		
		}
		pnonceout[0] = 0;
		phashstartout[0] = 0;


	
	int tmp;
	int write_len;
	unsigned char buf[8];
	for (int kk=0; kk < 8; kk++)
	{
	unpack_uint64(header[kk],buf);
	write_len = write(fd,&buf,8);	
//printf("write_len:  %#010x\n", write_len);
	};
	
int recv_ok;

	do {
	recv_ok = recvserial(fd);
	} while (recv_ok==0);

//if (recv_ok>0){
printf("U%02d :  %#010x\n", thr_id, recv_ok);
	
		if (recv_ok==0x3fffffff){

		} else
		{
			pnonceout[0] = recv_ok- 129;

			nonceResult[0] = pnonceout[0];

			uint32_t nonce = *nonceResult;
			nonce = (((nonce & 0xFF000000) >> 24) | 
				((nonce & 0x00FF0000) >> 8) | 
				((nonce & 0x0000FF00) << 8) | 
				((nonce & 0x000000FF) << 24));

			pthread_mutex_lock(&stratum_sock_lock);
			lastnonce[3] = lastnonce[2];
			lastnonce[2] = lastnonce[1];
			lastnonce[1] = lastnonce[0];
			lastnonce[0] = nonceResult[0];

			if (nonce != lastNonce1)
			{
				pUCP->submitWork(jobId, timestamp, nonce);
				lastNonce1 = nonce;
			}
			else
			{
				sprintf(line, "\t Same as lastNonce {%#08lx} [nonce: %#08lx]", lastNonce1, nonce);
				cout << line << endl;
			}

			pthread_mutex_unlock(&stratum_sock_lock);
			nonceResult[0] = 0;


			// Hash coming from GPU is reversed
			uint64_t hashFlipped = 0;
			hashFlipped |= (hashStart[0] & 0x00000000000000FF) << 56;
			hashFlipped |= (hashStart[0] & 0x000000000000FF00) << 40;
			hashFlipped |= (hashStart[0] & 0x0000000000FF0000) << 24;
			hashFlipped |= (hashStart[0] & 0x00000000FF000000) << 8;
			hashFlipped |= (hashStart[0] & 0x000000FF00000000) >> 8;
			hashFlipped |= (hashStart[0] & 0x0000FF0000000000) >> 24;
			hashFlipped |= (hashStart[0] & 0x00FF000000000000) >> 40;
			hashFlipped |= (hashStart[0] & 0xFF00000000000000) >> 56;


			sprintf(line, "\t Share Found @ 2^32! {%#08lx} [nonce: %#08lx]", hashFlipped, nonce);
			cout << line << endl;
			vprintf("Logging\n");
			Log::info(line);
			vprintf("Done logging\n");
			vprintf("Made line\n");

			numLines++;
		}


//} else
//{
//printf("U2:  %#010x\n", recv_ok);
//}
	//close(fd);        //close the com port

//sleep(100000);


		//uint32_t nonceStart = (uint64_t)lastNonceStart + (blocksize * 128 * threadsPerBlock);
		//lastNonceStart = nonceStart;
		//pnoncestart[0] = startNonce;
		vprintf("Wrote buffers...\n");
//kernel_vblake(pnoncestart, pnonceout, phashstartout, pheaderin);

/*
		hashStart[0] = phashstartout[0];
		pthread_mutex_lock(&stratum_log_lock);
		unsigned long long totalTime = std::time(0) - startTime;
		pthread_mutex_unlock(&stratum_log_lock);
		//hashes =00ffFFFF;//+ ((uint32_t)blocksize * (uint32_t)threadsPerBlock * WORK_PER_THREAD);
		if ((uint64_t)startNonce + (uint64_t)(blocksize * threadsPerBlock * WORK_PER_THREAD) < (uint64_t)end_nonce) {
			startNonce += ((uint32_t)blocksize * (uint32_t)threadsPerBlock * WORK_PER_THREAD);
		}
		else
			startNonce = 0x20000000u * (uint32_t)thr_id;

		double hashSpeed = (double)hashes;
		hashSpeed = 16777215/totalTime  ;

		if (count % 20 == 0) {
			pthread_mutex_lock(&stratum_sock_lock);

			int validShares = pUCP->getValidShares();
			int invalidShares = pUCP->getInvalidShares();
			int totalAccountedForShares = invalidShares + validShares;
			int totalSubmittedShares = pUCP->getSentShares();
			int unaccountedForShares = totalSubmittedShares - totalAccountedForShares;

			pthread_mutex_unlock(&stratum_sock_lock);

			double percentage = ((double)validShares) / totalAccountedForShares;
			percentage *= 100;
			// printf("[GPU #%d (%s)] : %f MH/second    valid shares: %d/%d/%d (%.3f%%)\n", deviceToUse, selectedDeviceName.c_str(), hashSpeed, validShares, totalAccountedForShares, totalSubmittedShares, percentage);
			//printf("hashend %#018llx \n ", header[0]);
			printf("[GPU #%d (%s)] : %0.2f MH/s shares: %d/%d/%d (%.3f%%)\n", thr_id, device_name[thr_id], hashSpeed, validShares, totalAccountedForShares, totalSubmittedShares, percentage);


		}

		if (nonceResult[0] != 0x01000000 && nonceResult[0] != 0
			&& lastnonce[0] != nonceResult[0] && lastnonce[1] != nonceResult[0] && lastnonce[2] != nonceResult[0]
			&& lastnonce[3] != nonceResult[0]) {

			uint32_t nonce = *nonceResult;
			nonce = (((nonce & 0xFF000000) >> 24) | ((nonce & 0x00FF0000) >> 8) | ((nonce & 0x0000FF00) << 8) | ((nonce & 0x000000FF) << 24));
			pthread_mutex_lock(&stratum_sock_lock);
			lastnonce[3] = lastnonce[2];
			lastnonce[2] = lastnonce[1];
			lastnonce[1] = lastnonce[0];
			lastnonce[0] = nonceResult[0];


			pUCP->submitWork(jobId, timestamp, nonce);
			pthread_mutex_unlock(&stratum_sock_lock);
			nonceResult[0] = 0;

			char line[100];

			// Hash coming from GPU is reversed
			uint64_t hashFlipped = 0;
			hashFlipped |= (hashStart[0] & 0x00000000000000FF) << 56;
			hashFlipped |= (hashStart[0] & 0x000000000000FF00) << 40;
			hashFlipped |= (hashStart[0] & 0x0000000000FF0000) << 24;
			hashFlipped |= (hashStart[0] & 0x00000000FF000000) << 8;
			hashFlipped |= (hashStart[0] & 0x000000FF00000000) >> 8;
			hashFlipped |= (hashStart[0] & 0x0000FF0000000000) >> 24;
			hashFlipped |= (hashStart[0] & 0x00FF000000000000) >> 40;
			hashFlipped |= (hashStart[0] & 0xFF00000000000000) >> 56;


			sprintf(line, "\t Share Found @ 2^32! {%#08lx} [nonce: %#08lx]", hashFlipped, nonce);


			cout << line << endl;
			vprintf("Logging\n");
			Log::info(line);
			vprintf("Done logging\n");
			vprintf("Made line\n");

			numLines++;

			// Uncomment these lines to get access to this data for display purposes
			
			//long long extraNonce = ucpClient.getStartExtraNonce();
			//int jobId = ucpClient.getJobId();
			//int encodedDifficulty = ucpClient.getEncodedDifficulty();
			//string previousBlockHashHex = ucpClient.getPreviousBlockHash();
			//string merkleRoot = ucpClient.getMerkleRoot();
			

		}*/
		vprintf("About to restart loop...\n");
	}

	printf("Resetting device...\n");

	//	free(pnoncestart);
	//free(pnonceout);
	//free(phashstartout);
	//free(pheaderin);
	getchar();
	return 0;


}

int main(int argc, char *argv[])
{
	// Check for help argument (only -h)

	for (int i = 1; i < argc; i++) {
		char* argument = argv[i];

		if (!strcmp(argument, "-h"))
		{
			printHelpAndExit();
		}
	}

	if (argc % 2 != 1) {
		sprintf(outputBuffer, "GPU miner must be provided valid argument pairs!");
		cerr << outputBuffer << endl;
		printHelpAndExit();
	}

	string hostAndPort = ""; //  "94.130.64.18:8501";
	string username = ""; // "VGX71bcRsEh4HZzhbA9Nj7GQNH5jGw";
	string password = "";

	if (argc > 1)
	{
		for (int i = 1; i < argc; i += 2)
		{
			char* argument = argv[i];
			printf("%s\n", argument);
			if (argument[0] == '-' && argument[1] == 'd')
			{

				int device_thr[MAX_GPUS] = { 0 };
				char ngpus = 0;//clDevicesNum();
				char* pch = strtok(argv[i + 1], ",");
				opt_n_threads = 0;
				while (pch != NULL && opt_n_threads < MAX_GPUS) {
					if (pch[0] >= '0' && pch[0] <= '9' && strlen(pch) <= 2)
					{
						if (atoi(pch) < ngpus)
							device_map[opt_n_threads++] = atoi(pch);
						else {
							printf("Non-existant device #%d specified in -d option\n\n", atoi(pch));
							printHelpAndExit();
						}
					}
					pch = strtok(NULL, ",");
				}
				// count threads per gpu
				for (int n = 0; n < opt_n_threads; n++) {
					int device = device_map[n];
					device_thr[device]++;
				}
				for (int n = 0; n < ngpus; n++) {
					gpu_threads = max(gpu_threads, device_thr[n]);
				}

				//  if (strlen(argv[i + 1]) == 2) {
				//  device num >= 10
				//	deviceToUse = (argv[i + 1][0] - 48) * 10 + (argv[i + 1][1] - 48);
				//  }
				//  else {
				//	deviceToUse = argv[i + 1][0] - 48;
				//  }
			}
			else if (!strcmp(argument, "-o"))
			{
				hostAndPort = string(argv[i + 1]);
			}
			else if (!strcmp(argument, "-u"))
			{
				username = string(argv[i + 1]);
			}
			else if (!strcmp(argument, "-p"))
			{
				password = string(argv[i + 1]);
			}
			else if (!strcmp(argument, "-tpb"))
			{
				threadsPerBlock = stoi(argv[i + 1]);
			}
			else if (!strcmp(argument, "-bs"))
			{
				blocksize = stoi(argv[i + 1]);
			}
			else if (!strcmp(argument, "-frm"))
			{
				blocksize = stoi(argv[i + 1]);
			}
			else if (!strcmp(argument, "-l"))
			{
				// to lower case conversion
				for (int j = 0; j < strlen(argv[i + 1]); j++)
				{
					argv[i + 1][j] = tolower(argv[i + 1][j]);
				}
				if (!strcmp(argv[i + 1], "true") || !strcmp(argv[i + 1], "t"))
				{
					Log::setEnabled(true);
				}
				else
				{
					Log::setEnabled(false);
				}
			}
			else if (!strcmp(argument, "-v"))
			{
				// to lower case conversion
				for (int j = 0; j < strlen(argv[i + 1]); j++)
				{
					argv[i + 1][j] = tolower(argv[i + 1][j]);
				}
				if (!strcmp(argv[i + 1], "true") || !strcmp(argv[i + 1], "t"))
				{
					verboseOutput = true;
				}
				else
				{
					verboseOutput = false;
				}
			}
		}
	}
	else {
		printHelpAndExit();
	}
	pthread_mutex_init(&stratum_sock_lock, NULL);
	pthread_mutex_init(&stratum_log_lock, NULL);

	if (HIGH_RESOURCE) {
		sprintf(outputBuffer, "Resource Utilization: HIGH");
		cerr << outputBuffer << endl;
		Log::info(outputBuffer);
	}
	else {
		sprintf(outputBuffer, "Resource Utilization: LOW");
		cerr << outputBuffer << endl;
		Log::info(outputBuffer);
	}


#ifdef _WIN32
	//HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#else
#endif

	if (hostAndPort.compare("") == 0) {
		string error = "You must specify a host in the command line arguments! Example: \n-o 127.0.0.1:8501 or localhost:8501";
		cerr << error << endl;
		Log::error(error);
		promptExit(-1);
	}

	if (username.compare("") == 0) {
		string error = "You must specify a username in the command line arguments! Example: \n-u V5bLSbCqj9VzQR3MNANqL13YC2tUep";
		cerr << error << endl;
		Log::error(error);
		promptExit(-1);
	}

	string host = hostAndPort.substr(0, hostAndPort.find(":"));
	//GetHostByName
	net_init();
	host = net_dns_resolve(host.c_str());
	net_deinit();

	string portString = hostAndPort.substr(hostAndPort.find(":") + 1);

	// Ensure that port is numeric
	if (portString.find_first_not_of("1234567890") != string::npos) {
		string error = "You must specify a host in the command line arguments! Example: \n-o 127.0.0.1:8501 or localhost:8501";
		cerr << error << endl;
		Log::error(error);
		promptExit(-1);
	}

	int port = stoi(portString);

	sprintf(outputBuffer, "Attempting to mine to pool %s:%d with username %s and password %s...", host.c_str(), port, username.c_str(), password.c_str());
	cout << outputBuffer << endl;
	Log::info(outputBuffer);
	UCPClient ucpClient(host, port, username, password);

	pUCP = &ucpClient;

	sprintf(outputBuffer, "Using Device: %d\n\n", deviceToUse);
	cout << outputBuffer << endl;
	Log::info(outputBuffer);
//sleep(10000);
	int version, ret;


	fd = openserial(MODEMDEVICE);//		serial_port	

	for (int i = 0; i < opt_n_threads; i++)
	{
		//	if (scan_platform(platform_M, &nr_devs_total, &plat_id, &dev_id[i], device_map[i]))

		/* Create context.
		context[i] = clCreateContext(NULL, 1, &dev_id[i],
			NULL, NULL, &status);
		if (status != CL_SUCCESS || !context[i]) {
			sprintf(outputBuffer, "clCreateContext (%d)", status);
			cout << outputBuffer << endl;
			Log::error(outputBuffer);
			promptExit(-1);
		}*/
		/* Creating command queue associate with the context.
		queue[i] = clCreateCommandQueue(context[i], dev_id[i],
			0, &status);
		if (status != CL_SUCCESS || !queue[i]) {
			sprintf(outputBuffer, "clCreateCommandQueue (%d)", status);
			cout << outputBuffer << endl;
			Log::error(outputBuffer);
			promptExit(-1);
		}*/

		/* Create program object */
#ifdef WIN32
		//load_file("input.cl", (char **)&source, &source_len, 0);
		//load_file("input.bin", &binary, &binary_len, 1);
#else
		//source = ocl_code;
#endif
		//source_len = strlen(source);
/*
		program[i] = clCreateProgramWithSource(context[i], 1, (const char **)&source,&source_len, &status);
			
		if (status != CL_SUCCESS || !program[i]) {
			sprintf(outputBuffer, "clCreateProgramWithSource (%d)", status);
			cout << outputBuffer << endl;
			Log::error(outputBuffer);
			promptExit(-1);
		}
*/
		/* Build program. */
		sprintf(outputBuffer, "Building program #%d", i);
		cout << outputBuffer << endl;
		Log::info(outputBuffer);
/*
		status = clBuildProgram(program[i], 1, &dev_id[i],
			(amd_flag) ? ("-I .. -I .") : ("-I .. -I ."), // compile options
			NULL, NULL);
		if (status != CL_SUCCESS) {
			sprintf(outputBuffer, "OpenCL build failed (%d). Build log follows:", status);

			get_program_build_log(program[i], dev_id[i]);
			cout << outputBuffer << endl;
			Log::error(outputBuffer);
			promptExit(-1);
		}
*/
//TODOS		get_program_bins(program[i]);
		// Create kernel objects
//TODOS		k_vblake[i] = clCreateKernel(program[i], "kernel_vblake", &status);
	//	if (status != CL_SUCCESS || !k_vblake[i]) {
	//		sprintf(outputBuffer, "clCreateKernel (%d)", status);
	//		cout << outputBuffer << endl;
	//		Log::error(outputBuffer);
//TODOS			get_program_build_log(program[i], dev_id[i]);
	//		promptExit(-1);
	//	}

	}

	pthread_t tids[MAX_GPUS];
	struct mining_attr m_args[MAX_GPUS];

	for (int i = 0; i < opt_n_threads; i++) {

		m_args[i].dev_id = device_map[i];

		pthread_create(&tids[i], NULL, miner_thread, &m_args[i]);
	}

	for (int i = 0; i < opt_n_threads; i++) {
		pthread_join(tids[i], NULL);
	}


}
