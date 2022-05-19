#include "api.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <map>


struct trans{
    std::string source;
    std::string symbols;
    std::string destination;
};

std::map<std::string,bool> form_states(std::vector<std::string> states,DFA &d) {
    std::map<std::string,bool> state_map;
    for(auto it:states){
        if(d.is_final(it))
            state_map[it] = true;
        else{
            state_map[it] = false;
        }
    }
    return state_map;
}
std::map<std::string,int> form_trans_map(std::vector<struct trans> trans,std::map<std::string,bool> state_map){
    std::map<std::string,int> map;
    for(auto it:state_map){
        map[it.first] = 0;
    }
    for(auto it:trans){
        if(it.source!="start" && it.destination!="end") {
            map[it.source]++;
            map[it.destination]++;
        }
    }
    return map;
}
std::string get_state_to_delete(std::map<std::string,int> trans_map){
    int current_min = INT16_MAX;
    std::string current_min_str;
    for(auto it:trans_map){
        if(it.second < current_min && it.second > 0){
            current_min = it.second;
            current_min_str = it.first;
        }
    }
    return current_min_str;
}

bool finish(std::vector<struct trans> trans_table){
    for(auto it:trans_table){
        if(it.source=="start" && it.destination == "end")
            return false;
    }
    return true;
}
std::string get_finish(std::vector<struct trans> trans_table){
    for(auto it:trans_table){
        if(it.source=="start" && it.destination == "end")
            return it.symbols;
    }
    return "";
}
std::string dfa2re(DFA &d) {
    auto set_states = d.get_states();
    std::vector<std::string> states;
    for(auto it:set_states){
        states.push_back(it);
    }
    for(auto it:states){
        std::cout << it << "\n";
    }
    std::cout << "\n";

    std::cout << "Start FILL STATES_MAP\n";
    auto states_map = form_states(states, d);
    std::cout << "FILL STATES_MAP\n";


    auto ALP = d.get_alphabet();
    ALP.insert('@');
    DFA my_table(ALP);
    my_table.create_state("start");
    my_table.set_initial("start");
    std::vector<trans> trans_table;
    trans trans_tmp;
    for(auto it:states_map){
        my_table.create_state(it.first);
    }
    my_table.set_trans("start",'@',states_map.begin()->first);
    trans_tmp.source="start";
    trans_tmp.symbols="()";
    trans_tmp.destination=states_map.begin()->first;
    trans_table.push_back(trans_tmp);
    my_table.create_state("end", true);
    std::cout << "FILL TRANSTABLE\n";
    for(auto it:d.get_alphabet().to_string()){
        for(auto iter:states_map){
            if(d.has_trans(iter.first,it)){
                my_table.set_trans(iter.first,it,d.get_trans(iter.first,it));
                trans_tmp.source=iter.first;
                trans_tmp.symbols=it;
                trans_tmp.destination=d.get_trans(iter.first,it);
                trans_table.push_back(trans_tmp);
            }
        }
    }

    for(auto it:states_map){
        if(it.second){
            trans_tmp.source=it.first;
            trans_tmp.symbols="()";
            trans_tmp.destination="end";
            trans_table.push_back(trans_tmp);
        }
    }
    auto trans_map = form_trans_map(trans_table,states_map);




    // удалить мертвые состояния?
    std::vector<trans> trans_table_loop;
    std::vector<std::string> order;
    std::cout << "MAIN\n";
    while(!trans_map.empty()){
        int min = INT16_MAX;
        std::string min_str;
        std::map<std::string , int>::iterator iter;
        for(auto it=trans_map.begin();it!=trans_map.end();++it){
            if(min > it->second){
                min = it->second;
                min_str = it->first;
                iter = it;
            }
        }
        order.push_back(min_str);
        trans_map.erase(iter);
    }
    std::vector<struct trans> trans_loop;
//    std::map<std::string,int> map;
//    for(auto it:states_map){
//        map[it.first] = 0;
//    }
//    for(auto it:trans_table){
//        if(it.source!="start") {
//            map[it.source]++;
//        }
//    }
//    std::vector<std::string> list_to_delete;
//    for(auto it:map){
//        if(it.second == 0 && it.first != "end"){
//            list_to_delete.push_back(it.first);
//        }
//    }
//    for(auto it:list_to_delete){
//        for(auto iter=trans_table.begin();iter > trans_table.end(); ++iter){
//            if(iter->source == it || iter->destination == it) {
//                trans_table.erase(iter);
//                iter--;
//            }
//        }
//    }
    bool flag;
    do{
        flag = false;
        for (auto it = trans_table.begin(); it < trans_table.end(); ++it) {
            std::cout << "it: " << it->source << ' ' << it->symbols << ' ' << it->destination << '\n';
            for (auto iter = it + 1; iter < trans_table.end(); ++iter) {
                std::cout << "iter: " << iter->source << ' ' << iter->symbols << ' ' << iter->destination << '\n';
                if ((iter->source == it->source) && (iter->destination == it->destination) &&
                    (iter->symbols != it->symbols)) {
//                    it->symbols = it->symbols ;
                    it->symbols.push_back('|');
//                    it->symbols =  it->symbols + "(" + iter->symbols + ")";
                    it->symbols += iter->symbols;
                    it->symbols = "("+it->symbols+")" ;
                    trans_table.erase(iter);
                    flag = true;
                }

            }
            std::cout << '\n';
        }
        for(auto it=trans_table.begin();it<trans_table.end();++it){
            if(it->destination==it->source){
                trans_loop.push_back(*it);
                trans_table.erase(it);
                flag = true;
            }
        }
        if(!order.empty()) {
            std::string deleting =order[0];
            std::vector<struct trans> trans_incoming;
            std::vector<struct trans> trans_outgoing;
            for(auto it=trans_table.begin();it<trans_table.end();++it){
                if(it->destination==deleting){
                    trans_incoming.push_back(*it);
                    trans_table.erase(it);
                    it--;
                }
                if(trans_table.empty()){
                    break;
                }
                if(it->source==deleting){
                    trans_outgoing.push_back(*it);
                    trans_table.erase(it);
                    it--;
                }
            }
            std::string loop_str = "";
            for(auto it:trans_loop){
                if(it.source == deleting){
                    if(loop_str.empty()){
                        loop_str = "("+it.symbols+")*";
                    }
                    else{
                        loop_str += "|("+it.symbols+")*";
                        loop_str = "(" + loop_str + ")";
                     }
                }
            }

            for(auto it=trans_incoming.begin();it<trans_incoming.end();++it){
                for(auto iter=trans_outgoing.begin();iter<trans_outgoing.end();++iter){
                    // где то тут что-то с loop
                    struct trans tmp;
                    tmp.source = it->source;
                    tmp.destination = iter->destination;
                    tmp.symbols = "";
                    if(it->symbols != "()")
                        tmp.symbols = it->symbols;
                    tmp.symbols += loop_str;
                    if(iter->symbols != "()")
                        tmp.symbols+=iter->symbols;
                    tmp.symbols = "(" + tmp.symbols + ")";
                    trans_table.push_back(tmp);
                    flag = true;
                }
            }
            order.erase(order.begin());
        }
//        break;
    }while(finish(trans_table) || flag);

    return get_finish(trans_table);
}