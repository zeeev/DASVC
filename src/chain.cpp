#include "chain.hpp"

bool chain::addAlignment(int s, int e, int m){

    node * n = new node;
    n->matches      = double(m);
    n->overallScore = double(m);
    n->start        = s;
    n->end          = e;

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
        nodes[i]->index = i - 1;
    }

    for(int i = 1 ; i < nodes.size() - 1; i++){

        double maxScore = 0;

        for(int j = i - 1; j > 0; j--){

            if(nodes[i]->start >= nodes[j]->end){

                double tmp_score = nodes[j]->overallScore + (nodes[j]->end - nodes[i]->start);
                if(tmp_score > maxScore){
                    maxScore = tmp_score;
                }
                nodes[i]->children.push_back(nodes[j]);
            }
        }
        nodes[i]->overallScore += maxScore;
    }

    /*

    for(int i = 0 ; i < nodes.size(); i++){
        std::cerr << "NI: " << nodes[i]->index << " " << nodes[i]->start << " " << nodes[i]->end << " " << nodes[i]->matches << " " << nodes[i]->overallScore << std::endl;
    }

    */
    /* Adding all the aligments to the sink.
       They all end before the sink starts. */

    for(int i = nodes.size() - 2; i > 0; i--){
        nodes.back()->children.push_back(nodes[i]);
    }

    last = nodes.back();

    return true;
}

bool chain::traceback(std::vector<int> & alns){


    /*        std::cerr << " tracking back : "
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


    double max = last->children.front()->overallScore;
    node * current = last->children.front();

    for(std::vector<node *>::iterator it = last->children.begin();
        it != last->children.end(); it++){
        if((*it)->overallScore > max ){
            /*            std::cerr << " child: "
                      << (*it)->index
                      << " " << (*it)->start
                      << " " << (*it)->end
                      << " " << (*it)->overallScore
                      << " past " << current->index
                      << " " << max << std::endl;

            */
            max       = (*it)->overallScore;
            current   =  *it ;

        }



    }

    if(last != nodes.back() && last != nodes.front()){
        alns.push_back(current->index);
    }
    last = current;

    return traceback(alns);
}
