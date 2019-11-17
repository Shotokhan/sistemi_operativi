#define P1 1
#define P2 2

#define FTOK_PATH_Q "./"
#define FTOK_CHAR_Q 'b'

#define P1_N_MSG 5
#define P2_N_MSG 5

typedef struct{
	long processo;
	float numero;
} msg_calc;

int open_queue(key_t keyval);
