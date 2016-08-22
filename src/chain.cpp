#include "chain.hpp"

bool chain::addAlignment(int s, int e, int m){

    node * n = new node;
    n->matches      = m;
    n->start        = s;
    n->end          = e;
    n->overallScore = m;

    nodes.push_back(n);

    return true;
}

bool _endCmp(const node * L, const node * R){
    return L->end < R->end;
}

chain::chain(void){
    /* setting up the source node */
    addAlignment(-1, -1, -1);

}

chain::~chain(void){
    for(std::vector<node *>::iterator it = nodes.begin();
        it != nodes.end(); it++){
        delete *it;
    }
}

bool chain::_buildLinks(void){

    // sorted by end position
    sort(nodes.begin(), nodes.end(), _endCmp);

    // adding sink

    addAlignment(nodes.back()->end + 1, nodes.back()->end +2, -1);

    // indexing souce and sink

    nodes.front()->index = -1;
    nodes.back()->index  = -1;

    /* Go from the first real aligment to the last.
       No source or sink.
     */

    for(int i = 1 ; i < nodes.size() - 1; i++){
        nodes[i]->index = i - 1;
        for(int j = i ; j > 0; j--){
            if(nodes[i]->start >= nodes[j]->end){
                nodes[i]->overallScore
                    += nodes[j]->overallScore
                    + (nodes[j]->end - nodes[i]->start);
                nodes[i]->children.push_back(nodes[j]);
            }
        }
    }

    /* Adding all the aligments to the sink.
       They all end before the sink starts. */

    for(int i = nodes.size() - 2; i > 0; i--){
        nodes.back()->children.push_back(nodes[i]);
    }

    last = nodes.back();

    return true;
}

bool chain::traceback(std::vector<int> & alns){

    int max = -1;

    node * current;

    for(std::vector<node *>::iterator it = last->children.begin();
        it != last->children.end(); it++){
        if(max > (*it)->overallScore){
            max       = (*it)->overallScore;
            current   = *it ;
        }
    }


    if(current->children.empty()){
        return false;
    }
    else{
        alns.push_back(current->index);
    }

    return traceback(alns);
}
