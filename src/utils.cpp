#include <vector>
#include <set>
#include <stdio.h>
#include "utils.h"
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
            
            #ifdef MPZ_BIGINT
                if (mpz_tstbit(y.get_mpz_t(), 0)) t|=1;

            #else
                if (y&1) t|=1;
            #endif

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
            
            #ifdef MPZ_BIGINT
                if (mpz_tstbit(y.get_mpz_t(), 0)) ADDONEEDGE(g,i,j,m);
            #else
                if (y&1) ADDONEEDGE(g,i,j,m);
            #endif
            
            y>>=1;
        }
    }

}

/*
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
*/

BIGINT g6_to_y(char* s, int n){

    int m = (n + WORDSIZE - 1) / WORDSIZE;
    
    stringtograph(s, g, m);

    return graph_to_y(g, n);
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

BIGINT canon_label(BIGINT y, int nv){

    // get number of vertices and m
    int m = (nv + WORDSIZE - 1) / WORDSIZE;
    //nauty_check(WORDSIZE, m, nv, NAUTYVERSIONID);

    // convert string to graph
    //y_to_g6(y, nv, buffer);
    //stringtograph(buffer, g, m);

    // convert y to graph
    y_to_graph(y, nv, g);

    // set canonical flag
    options.getcanon = TRUE;

    // canonically label graph
    nauty(g, lab, ptn, NULL, orbits, &options, &stats, workspace, 64 * m, m, nv, canon_g);

    return graph_to_y(canon_g, nv);
    //strcpy(buffer, ntog6(canon_g, m, nv));
    //return g6_to_y(buffer);

}

void get_combinations(std::vector<BIGINT>& v, int n, int r, std::vector<int>& c, int st){

    if (r==0){


        int x,y;

        BIGINT one = 1;

        /*
        for (x=0;x<c.size();x++){
            printf("%d ",c[x]);

        }
        printf("\n");
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
        
        #ifdef MPZ_BIGINT
            if (mpz_tstbit(n.get_mpz_t(), 0)) s="1"+s;
        #else
            if (n&one) s="1"+s;
        #endif
        
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

bool file_exists(const char* filename){
    FILE* fp = NULL;

    fp = fopen(filename, "r");

    if(fp != NULL){
        fclose(fp);
        return true;
    }

    return false;
}


void write_to_file_g6(std::set<BIGINT> *graphs_ptr, int vertices, const char* filename){

    char graph_g6_str[2048];
    char command[100];

    // open file for writing
    FILE *fp;

    fp = fopen(filename, "w+");

    //std::cout << "Writing " << graphs_ptr->size() << " graphs." << std::endl;

    for (std::set<BIGINT>::iterator it=graphs_ptr->begin(); it != graphs_ptr->end(); ++it){
        y_to_g6(*it, vertices, graph_g6_str);
        fputs(graph_g6_str, fp);
    }

    fclose(fp);

    // sort the file
    sprintf(command, "LC_ALL=C sort -o '%s' '%s'", filename, filename);
    int r = system(command);

}