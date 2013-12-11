#include <iostream>
#include <vector>
#include "solver.h"

#define MAXN 64
#include "nauty.h"
#include "gtools.h"

int m2a_map[MAX_VERTEX][MAX_VERTEX];

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

    *s = '\0';

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

void get_combinations(std::vector<BIGINT>& v, int n, int r, std::vector<int>& c, int st=-1){

    if (r==0){


        int x,y;

        BIGINT one = 1;

        /*
        for (x=0;x<c.size();x++){
            cout << c[x] << " ";

        }
        cout << endl;
        */
        
        // find all edges in c and create edge number
        BIGINT t, tmp;
        t=0;        
        for (x=0;x<c.size();x++){
            for (y=x+1;y<c.size();y++){
                // set bit for edge x-y
                tmp = 1;
                tmp<<=m2a_map[c[x]][c[y]];
                t |= tmp;
            }
        }
        v.push_back(t);
        return;
    }

    for (int i=st+1;i<n;i++){
        c.push_back(i);
        get_combinations(v,n,r-1,c,i);
        c.pop_back();
    }

}

std::string binary_str(BIGINT n, int bits){

    std::string s="";
    BIGINT one = 1;

    int current_vertex = 1;
    int count=current_vertex;
    for (int i=0;i<bits;i++){
        if (n&one) s="1"+s;
        else s="0"+s;

        count--;

        if (count==0){
            s=" "+s;
            current_vertex++;
            count=current_vertex;
        }

        n>>=1;
    }

    return s;

}