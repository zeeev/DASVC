#ifndef CHAIN_H
#define CHAIN_H

#include <vector>
#include <algorithm>

class node{
public:
    int matches     ;
    int start       ;
    int end         ;
    int overallScore;
    int index       ;

    std::vector<node *> children;

};

class chain{
private:
    node               * last;
    std::vector<node *> nodes;
    bool _buildLinks(  void    );
public:
    chain(void);
    ~chain(void);
    bool addAlignment(int, int, int         );
    bool traceback(std::vector<int> &       );
};

    bool _endCmp(const node *, const node * );

#endif
