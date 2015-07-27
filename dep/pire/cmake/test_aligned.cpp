// every object of type sse_t will be aligned to 16-byte boundary
struct __attribute__((aligned)) sse_t
{
  float sse_data[4];
};

int main(int argc, char *argv[]){return 0;}
