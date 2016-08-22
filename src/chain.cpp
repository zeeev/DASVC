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
    addAlignment(-1, -1, 0);

}

chain::~chain(void){
    for(std::vector<node *>::iterator it = nodes.begin();
        it != nodes.end(); it++){
        delete *it;
    }
}

bool chain::buildLinks(void){

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

    for(int i = 0; i < nodes.size(); i++){
        nodes[i]->index = i;
    }

    for(int i = 1 ; i < nodes.size() - 1; i++){
        for(int j = i - 1; j > 0; j--){
            if(nodes[i]->start >= nodes[j]->end){
                nodes[i]->overallScore
                    += nodes[j]->overallScore
                    + (nodes[j]->end - nodes[i]->start);
                nodes[i]->children.push_back(nodes[j]);
                break;
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


    /*    std::cerr << " tracking back : "
              << " " << last->index
              << " " << last->start
              << " " << last->end
              << " " << last->matches
              << " " << last->overallScore
              << std::endl;

    */

    if(last->children.empty()){
        return false;
    }


    int max = last->children.front()->overallScore;
    node * current = last->children.front();

    for(std::vector<node *>::iterator it = last->children.begin();
        it != last->children.end(); it++){
        if((*it)->overallScore > max ){
            max       = (*it)->overallScore;
            current   =  *it ;
        }
        //        std::cerr << "child : " <<  (*it)->start << " " << (*it)->end << " " << (*it)->overallScore << std::endl;
    }

    if(last != nodes.back()){
        alns.push_back(current->index);
    }
    last = current;

    return traceback(alns);
}
