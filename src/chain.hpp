#ifndef CHAIN_H
#define CHAIN_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
class node{
public:
    double matches       ;
    double overallScore  ;
    int start            ;
    int end              ;
    int index            ;

    std::vector<node *> children;

};

class chain{
private:
    int current_index        ;
    node               * last;
    std::vector<node *> nodes;

public:
    chain(void);
    ~chain(void);
    bool buildLinks(  void    );
    bool addAlignment(int, int, int         );
    bool traceback(std::vector<int> &       );
};

    bool _endCmp(const node *, const node * );

#endif
