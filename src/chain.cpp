#include "chain.hpp"

bool chain::addAlignment(int s, int e, int m){

    node * n = new node;
    n->matches      = double(m);
    n->overallScore = double(m);
    n->start        = s;
    n->end          = e;
    n->index        = current_index;

    nodes.push_back(n);

    current_index++;

    return true;
}

bool _endCmp(const node * L, const node * R){
    return L->end < R->end;
}

chain::chain(void){
    /* setting up the source node */
    current_index = -1     ;
    addAlignment(-2, -1, 0);
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
    addAlignment(nodes.back()->end + 1, nodes.back()->end +2, 0);

    for(int i = 1 ; i < nodes.size() ; i++){

        double maxScore = 0;
        for(int j = i - 1; j >= 0; j--){
            if(nodes[j]->end <= nodes[i]->start){
                double tmp_score = nodes[j]->overallScore ;
                if(tmp_score > maxScore){
                    maxScore = tmp_score;
                }
                nodes[i]->children.push_back(nodes[j]);
            }
        }
        nodes[i]->overallScore += maxScore;
    }

    last = nodes.back();

    return true;
}

bool chain::traceback(std::vector<int> & alns){

    if(last != nodes.front() && last != nodes.back() ){
        alns.push_back(last->index);
    }

    if( last->children.empty() ){
        return false;
    }

    double max = last->children.front()->overallScore;
    node * current = last->children.front();

    for(std::vector<node *>::iterator it = last->children.begin();
        it != last->children.end(); it++){

        if((*it)->overallScore > max ){

            max       = (*it)->overallScore;
            current   =  *it ;
        }
    }



    std::cerr << "  best child : "
              << " " << current->index
              << " " << current->start
              << " " << current->end
              << " " << current->matches
              << " " << current->overallScore
              << std::endl;




    last = current;

    return traceback(alns);
}
