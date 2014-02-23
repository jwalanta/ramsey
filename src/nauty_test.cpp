#include <iostream>
#include <string>
#include <string.h>
#include <set>

#define MAXN 64
#include "nauty.h"
#include "gtools.h"

//#define BIGINT __uint128_t
#define BIGINT long long

// declarations for nauty
graph g[MAXN*MAXM];
graph canon_g[MAXN*MAXM];
int lab[MAXN],ptn[MAXN],orbits[MAXN];
static DEFAULTOPTIONS_GRAPH(options);
statsblk stats;
setword workspace[64*MAXM];

// buffer char, used in graph format conversions
// declared here for performance
char buffer[1000];


void y_to_g6(BIGINT y, int n, char* s){

    int i;
    char t;

    int m = n*(n-1)/2;

    if (n<=62){
        *s++ = (char)(n + 63);
    }

    while (m>0){

        t=0;
        // get six bits from back
        for (i=0;i<6;i++){

            t<<=1;

            // check if last bit is set
            if (y&1) t|=1;

            y>>=1;
        }

        t+=63;

        *s++ = t;

        m-=6;
    }

    *s++ = '\n';
    *s = '\0';

}

void y_to_graph(BIGINT y, int n, graph *g){

    int i,j, count=0;
    int m = (n + WORDSIZE - 1) / WORDSIZE;

    EMPTYGRAPH(g,m,n);

    for (j=1;j<n;j++){
        for (i=0;i<j;i++){
            if (y&1) ADDONEEDGE(g,i,j,m);
            y>>=1;
        }
    }

}

BIGINT graph_to_y(graph *g, int n){

    int i,j, count=0;
    int m = (n + WORDSIZE - 1) / WORDSIZE;
    set *gj;

    BIGINT y=0;

    for (j=n-1;j>0;j--){
        gj = GRAPHROW(g,j,m);
        for (i=j-1;i>=0;i--){
            y<<=1;
            if (ISELEMENT(gj,i)) y|=1;
        }
    }

    return y;

}


BIGINT g6_to_y(char* s){

    // get order of graph
    int n = (int)*s++;
    n-=63;
    int m = n*(n-1)/2;

    int shift = 0;

    int i;
    unsigned char c;

    BIGINT y=0, tmp;

    // compute edges
    while (*s != '\0' && *s != '\n'){   // nauty adds \n 

        c = *s++;
        c-=63;

        // first two bits
        c<<=2;

        // reverse the byte
        // http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64BitsDiv
        tmp = (c * 0x0202020202ULL & 0x010884422010ULL) % 1023;

        // shift it to its position
        tmp<<=shift;    

        // merge it to y
        y|=tmp;

        shift+=6;
    }

    return y;

}

BIGINT canon_label(BIGINT y, int nv){

    y_to_g6(y, nv, buffer);

    // get number of vertices and m
    int m = (nv + WORDSIZE - 1) / WORDSIZE;
    nauty_check(WORDSIZE, m, nv, NAUTYVERSIONID);

    // convert string to graph
    stringtograph(buffer, g, m);

    // set canonical flag
    options.getcanon = TRUE;

    // canonically label graph
    nauty(g, lab, ptn, NULL, orbits, &options, &stats, workspace, 64 * m, m, nv, canon_g);

    strcpy(buffer, ntog6(canon_g, m, nv));
    return g6_to_y(buffer);

}




int main(){

    char a[100], b[100];
    int nv = 10;
    BIGINT n;

    int m = (nv + WORDSIZE - 1) / WORDSIZE;

    for (BIGINT i=0;i<10;++i){ //65536L;i++){
        //y_to_g6(i,nv,a);
        //stringtograph(a, g, m);
        y_to_graph(i, nv, g);
        n = graph_to_y(g,nv);

        if (i!=n) std::cout << i << " " << n << " ";

        y_to_g6(i,nv,a);
        stringtograph(a, g, m);
        n = graph_to_y(g,nv);

        std::cout << i << " " << n << std::endl;        

        //y_to_graph(i, nv, g);
        //strcpy(b, ntog6(g, m, nv));

        //if (strcmp(a,b)!=0) std::cout << a << " " << b << " " << strcmp(a,b) << std::endl;
    }


    return 0;

    /*
    for (BIGINT i=64;i<4294967296L;i++){

        //std::cout << "N:" << i;
        y_to_g6(i,32,a);

        //std::cout << ", S:" << a;
        n=g6_to_y(a);

        //std::cout << ", N:" << n << std::endl;

        if (i!=n) {
            std::cout << "Error" << i << " " << n << std::endl;
            break;
        }
    }
    */

    std::set<BIGINT> gr;

    //int th_id = omp_get_thread_num();

    //std::cout << "Total threads: " << omp_get_num_threads() << std::endl;

    //for (BIGINT i=0;i<4294967296L;i++){
    //#pragma omp parallel for num_threads (4)
    for (BIGINT i=0;i<65536L;i++){
        gr.insert(canon_label(i,32));
    }

    //for (std::set<BIGINT>::iterator it=gr.begin(); it!=gr.end(); ++it)
    //    std::cout << ' ' << *it << std::endl;

    //for (int i=0;i<omp_get_num_threads();i++)
    std::cout << "Size: " << gr.size() << std::endl;

}
