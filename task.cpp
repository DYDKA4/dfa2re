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

std::vector<std::string> spliter_str(std::string str){
    std::string delimiter = "\n";
    std::vector<std::string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
//        std::cout << token << std::endl;
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
//    std::cout << str << std::endl;
    return result;
}
std::vector<std::string> get_state(std::vector<std::string> str, std::string alphabet){
    std::vector<std::string> states;
    for(auto iter:str){
        bool is_in = false;
        for(char it:alphabet){
            size_t pos = 0;
            pos = iter.find(it);
            if (pos != std::string::npos){
                is_in = true;
            }
        }
        if(is_in){
            break;
        }
        states.push_back(iter);
    }
    return states;
}

std::map<std::string,bool> form_states(std::vector<std::string> states) {
    std::map<std::string,bool> state_map;
    for(auto it:states){
        it.erase(it.begin());
        size_t pos = 0;
        if(it[0] == '['){
            it.erase(it.begin());
            pos = it.find(']');
            it.erase(it.begin()+pos,it.end());
            state_map[it] = true;
        }
        else{
            pos = it.find(']');
            it.erase(it.begin()+pos,it.end());
            state_map[it] = false;
        }
    }
    return state_map;
}
std::map<std::string,int> form_trans_map(std::vector<std::string> trans,std::map<std::string,bool> state_map){
    std::map<std::string,int> map;
    for(auto it:state_map){
        map[it.first] = 0;
    }
    for(auto it:trans){
        it.erase(it.begin());
        size_t pos = 0;
        pos = it.find(']');
        it.erase(it.begin()+pos,it.end());
        map[it]++;

    }
    return map;
}

std::string dfa2re(DFA &d) {
    std::string str = d.to_string();
    auto split_str = spliter_str(str);
    std::string alphabet = split_str[0];
    split_str.erase(split_str.begin());
    auto states = get_state(split_str,alphabet);
    for(auto it:states){
        std::cout << it << "\n";
    }
    std::cout << "\n";
    split_str.erase(split_str.begin(),split_str.begin()+states.size());
    for(auto it:split_str){
        std::cout << it << "\n";
    }
    auto states_map = form_states(states);
    auto trans_map = form_trans_map(split_str,states_map);

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
    trans_tmp.symbols='@';
    trans_tmp.destination=states_map.begin()->first;
    trans_table.push_back(trans_tmp);
    my_table.create_state("end", true);
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
            my_table.set_trans(it.first,'@',"end");
            trans_tmp.source=it.first;
            trans_tmp.symbols='@';
            trans_tmp.destination="end";
            trans_table.push_back(trans_tmp);
        }
    }
    // удалить мертвые состояния?
    std::vector<trans> trans_table_loop;
l:    do{
        for(auto it=trans_table.begin();it != trans_table.end();++it){
            std::cout <<"it: "<<it->source<< ' ' << it->symbols<< ' ' << it->destination << '\n';
            for(auto iter = it+1;iter!=trans_table.end();++iter){
                std::cout <<"iter: "<<iter->source<< ' ' << iter->symbols<< ' ' << iter->destination << '\n';
                if((iter->source == it->source) && (iter->destination==it->destination) && (iter->symbols!=it->symbols)){
                    it->symbols.push_back('|');
                    it->symbols += iter->symbols;
                    trans_table.erase(iter);
                    goto l;
                }

            }
            std::cout << '\n';
        }
    }while(false);

l2:    do {
    for (auto it = trans_table.begin(); it != trans_table.end(); ++it) {
        std::cout << "it: " << it->source << ' ' << it->symbols << ' ' << it->destination << '\n';
        if (it->source == it->destination) {
            trans_table_loop.push_back(*it);
            trans_table.erase(it);
            goto l2;
        }
    }
}while(false);
//    std::vector<trans> trans_table_outgoing;
//    std::vector<trans> trans_table_incoming;
//l3:    do {
//        std::string source,destination;
//        for (auto it = trans_table.begin(); it != trans_table.end(); ++it) {
//            std::cout << "it: " << it->source << ' ' << it->symbols << ' ' << it->destination << '\n';
//            if (it->source != "start" && it->source !="end") {
//                trans_table_loop.push_back(*it);
//                trans_table.erase(it);
//                goto l3;
//            }
//        }
//    }while(false);


  return my_table.to_string();
}
