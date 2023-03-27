#ifndef MYGLOBAL
#define MYGLOBAL

#define SAFE_DELETE(p) if(p){delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){delete [] p; p = NULL;}
#define SAFE_FREE(p) if(p){free(p); p = NULL;}

typedef unsigned char byte;


#endif // MYGLOBAL

