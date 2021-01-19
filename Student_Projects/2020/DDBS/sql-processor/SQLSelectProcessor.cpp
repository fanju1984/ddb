#include <iostream>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <sstream>

#include "../data-loader/Attribute.h"
#include "../data-loader/Fragment.h"
#include "../data-loader/Predicate.h"
#include "../data-loader/Relation.h"
#include "SelectStatement.h"

using namespace std;
//变量名小写，类名每个单词开头都大写,函数名和变量名一样
#define use_transport_optimization 1 //0: not use, 1:use

const int INF = 2147483647; //take the maximum of int as INF
// map<string,string> siteno_to_ip;


class TNAttribute{
public:
    string aname;
    int type; //1:Integer 2:String
    int value_type;
    //1:ID:no duplicate e.g.1-100
    //2:U:Uniform Distribution e.g.U[1 100]
    //3:PN:Positive Gaussian Distribution e.g.X~N(3,2)and X>0
    //4:Discrete:give corresponding key and proportion.
    vector<double> value; //Value for ValueType 1,2,3
    map<string, double> proportion; //Value for ValueType 4.
    bool has_restriction;
    vector<double> restriction_range; //restriction for ValueType 1,2,3
    string restriction;  //restriction for ValueType 4
};
class TreeNode{
public:
    int node_type; //1:Fragments 2:Join 3:Union
    string rname;
    string fname; //if node_type=1
    vector<string> join;  //if node_type=2 e.g.customer_id orders_cid
    bool has_selection;
    vector<Predicate> selection; //if has_selection=true
    bool has_projection;
    vector<string> projection; //if has_projection=true e.g. book_copies book_id
    string sname; //Name of executed site
    int num_of_recs;
    vector<TNAttribute> tn_attributes;
    int tn_size;
    string layer; // e.g._1_1_2, _1_2_4
    int  parent;  //-1 if no parent
    vector<int> child; //child[0]=-1 if no child
    void create_treenode(int n_t,string fna,string rna,string sna,vector<string> attribute,SelectStatement sql,vector<string> target, vector<Attribute> attri,int frag_type,vector<Predicate> hf_condition,int nor);
    void get_tn_size();
    void eliminate_attribute();
    void determine_projection(vector<string> target);
};
class Tree{
public:
    vector<TreeNode> tn;
    int n; //Total Number of Nodes in Tree
};

string  num2str( double  i)
{
    stringstream ss;
    ss<<i;
    return ss.str();
}

double NormalCDF(double z,double miu,double sigma){
    if(z==INF) return 1;
    else if(z==-INF) return 0;
    double result = 1.0/2.0*(1+erf((z-miu)/(sigma*sqrt(2))));
    return result;
}

void Predicate::get_interval(vector<double>& result){ //closed interval
    if(op_type==1) {result.push_back(num); result.push_back(INF);}
    if(op_type==2) {result.push_back(-INF); result.push_back(num);}
    if(op_type==3) {result.push_back(num+1); result.push_back(INF);}
    if(op_type==4) {result.push_back(-INF); result.push_back(num-1);}
    if(op_type==5) {result.push_back(num); result.push_back(num);}
}

void TreeNode::determine_projection(vector<string> target) {
    for(int j=0;j<tn_attributes.size();j++){
        if(find(target.begin(),target.end(),tn_attributes[j].aname)==target.end()){
            has_projection=true;
        }
        else{
            if(find(projection.begin(),projection.end(),tn_attributes[j].aname)==projection.end()) projection.push_back(tn_attributes[j].aname);
        }
    }
}

void TreeNode::eliminate_attribute() { //eliminate tn_attribute not in "projection" for this node
    if(has_projection==false) return;
    for(int k=0;k<tn_attributes.size();k++){
        if(find(projection.begin(),projection.end(),tn_attributes[k].aname) == projection.end() ){
            tn_attributes.erase(tn_attributes.begin()+k);
            k--;
        }
    }
}

void TreeNode::get_tn_size() {
    int i;
    tn_size=0;
    for(i=0;i<tn_attributes.size();i++){
        if(tn_attributes[i].type==1){
            tn_size+=sizeof(int);
        }
        else if(tn_attributes[i].type==2){
            tn_size+=sizeof(string);
        }
    }
    tn_size*=num_of_recs;
}

void get_intersection(vector<double> a,vector<double> b,vector<double>& result){ //closed interval
    result.push_back(max(a[0],b[0]));
    result.push_back(min(a[1],b[1]));
}

void TreeNode::create_treenode(int n_t,string fna,string rna,string sna,vector<string> attribute,SelectStatement sql,vector<string> target,vector<Attribute> attri,int frag_type,vector<Predicate> hf_condition, int nor){
    node_type=n_t;
    int k,ii,jj,kk;
    if(n_t==1){
        fname=fna;
        rname=rna;
        sname=sna;
        has_selection=false;
        for(k=0;k<attribute.size();k++){
            for(ii=0;ii<sql.where.size();ii++){
                if(attribute[k]==sql.where[ii].aname){
                    has_selection=true;
                    selection.push_back(sql.where[ii]);
                }
            }
        }
        has_projection=false;
        for(k=0;k<attribute.size();k++){
            vector<string>::iterator iter = find(target.begin(),target.end(),attribute[k]);
            if(iter == target.end()){
                has_projection=true;
            }
            else{
                if(find(projection.begin(),projection.end(),attribute[k])==projection.end()) projection.push_back(attribute[k]);
            }
        }
        //finish selection and projection
        set<string> s(projection.begin(),projection.end());
        projection.assign(s.begin(), s.end());
        //then remove repetition in projection
        for(k=0;k<attri.size();k++){
            TNAttribute temp_tna;
            temp_tna.aname=attri[k].aname;
            temp_tna.type=attri[k].type;
            temp_tna.value_type=attri[k].value_type;
            temp_tna.value=attri[k].value;
            temp_tna.proportion=attri[k].proportion;
            temp_tna.has_restriction=false;
            if(has_selection==true){
                for(jj=0;jj<selection.size();jj++){
                    if(temp_tna.aname==selection[jj].aname){
                        if(temp_tna.has_restriction==false){
                            temp_tna.has_restriction=true;
                            if(temp_tna.type==1&&temp_tna.value_type!=4){  //int
                                selection[jj].get_interval(temp_tna.restriction_range);
                            }
                            else{ //string
                                if(temp_tna.type==2){
                                    temp_tna.restriction=selection[jj].str;
                                }
                                else{
                                    temp_tna.restriction=num2str(selection[jj].num);
                                }
                            }
                        }
                        else{  //restriction already exist: only for int type(type=1)
                            vector<double> t2,ttt;
                            selection[jj].get_interval(t2);
                            get_intersection(temp_tna.restriction_range,t2,ttt);
                            temp_tna.restriction_range.erase(temp_tna.restriction_range.begin());
                            temp_tna.restriction_range.erase(temp_tna.restriction_range.begin());
                            temp_tna.restriction_range.push_back(ttt[0]);
                            temp_tna.restriction_range.push_back(ttt[1]);
                        }
                    }
                }
            }
            if(frag_type==2){ //horizontal frags should also consider hf_condition into tn_attribute restriction
                for(jj=0;jj<hf_condition.size();jj++){
                    if(temp_tna.aname==hf_condition[jj].aname){
                        if(temp_tna.has_restriction==false){
                            temp_tna.has_restriction=true;
                            if(temp_tna.type==1){  //int
                                 hf_condition[jj].get_interval(temp_tna.restriction_range);
                            }
                            else{ //string
                                 temp_tna.restriction=hf_condition[jj].str;
                            }
                        }
                        else{  //restriction already exist
                            if(temp_tna.type==1){ //for int type(type=1)
                                vector<double> t3,ttt;
                                hf_condition[jj].get_interval(t3);
                                get_intersection(temp_tna.restriction_range,t3, ttt);
                                temp_tna.restriction_range.erase(temp_tna.restriction_range.begin());
                                temp_tna.restriction_range.erase(temp_tna.restriction_range.begin());
                                temp_tna.restriction_range.push_back(ttt[0]);
                                temp_tna.restriction_range.push_back(ttt[1]);
                            }
                            else{  //for string type:selection and hf condition is the same, no need to select on this attribute
                                for(kk=0;kk<selection.size();kk++){
                                    if(selection[kk].aname==temp_tna.aname) selection.erase(selection.begin()+kk);
                                    break; //string type can only have less than one selection predicate
                                }
                                if(selection.size()==0) has_selection=false;
                            }
                        }
                    }
                }
            }
            tn_attributes.push_back(temp_tna);
        }
        num_of_recs=nor;
        for(k=0;k<tn_attributes.size();k++){
            if(tn_attributes[k].has_restriction==true){
                double prop;
                if(tn_attributes[k].value_type==1){
                    prop=(min(tn_attributes[k].restriction_range[1],tn_attributes[k].value[1])-max(tn_attributes[k].restriction_range[0],tn_attributes[k].value[0]))/(tn_attributes[k].value[1]-tn_attributes[k].value[0]);
                }
                else if(tn_attributes[k].value_type==2){
                    prop=(min(tn_attributes[k].restriction_range[1],tn_attributes[k].value[1])-max(tn_attributes[k].restriction_range[0],tn_attributes[k].value[0]))/(tn_attributes[k].value[1]-tn_attributes[k].value[0]);
                }
                else if(tn_attributes[k].value_type==3){ //positive Gaussian Distribution
                    double miu=tn_attributes[k].value[0];
                    double sigma=sqrt(tn_attributes[k].value[1]);
                    double denominator=NormalCDF(tn_attributes[k].restriction_range[1],miu,sigma)-NormalCDF(max(tn_attributes[k].restriction_range[0],0.0),miu,sigma);
                    double numerator=NormalCDF(INF,miu,sigma)-NormalCDF(0,miu,sigma);
                    prop=denominator/numerator;
                }
                else if(tn_attributes[k].value_type==4){
                    prop = tn_attributes[k].proportion.find(tn_attributes[k].restriction)->second;
                }
                num_of_recs=num_of_recs*prop;
            }
        }//get num_of_recs
        eliminate_attribute();
        parent=-1;
        child.push_back(-1);
    }
}

bool exist_conflict(Predicate a, Predicate b){
    if(a.op_type==6&&b.op_type==6){  //string type
        if(a.str!=b.str) return true;
    }
    else{  //number type
        vector<double> a_interval;
        a.get_interval(a_interval);
        vector<double> b_interval;
        b.get_interval(b_interval);
        if(a_interval[1]<b_interval[0]||a_interval[0]>b_interval[1]) return true;
    }
    return false;
}

void find_rres(vector<TNAttribute> tn_attributes,string aaname,vector<double>& result){
    int k;
    for(k=0;k<tn_attributes.size();k++){
        if(tn_attributes[k].aname==aaname){
            if(tn_attributes[k].has_restriction==true){
                result.insert(result.end(),tn_attributes[k].restriction_range.begin(),tn_attributes[k].restriction_range.end());
            }
            else{
                vector<double> tt;
                tt.push_back(-INF);
                tt.push_back(INF);
                result.insert(result.end(),tt.begin(),tt.end());
            }
            break;
        }
    }
}

string get_join_name(string x,string y){
    string result;
    if(x<y) result=x+"_"+y;
    else result=y+"_"+x;
    return result;
}

void print_node(TreeNode tn){
    int i,j,k;
    cout<<"node layer: "<<tn.layer<<endl;
    cout<<"node type: "<<tn.node_type<<endl;
    if(tn.node_type==1) cout<<"fname: "<<tn.fname<<endl;
    else if(tn.node_type==2) cout<<"join: "<<tn.join[0]<<" "<<tn.join[1]<<endl;
    cout<<"sname: "<<tn.sname<<endl;
    cout<<"has_projection: "<<tn.has_projection<<endl;
    if(tn.has_projection==true) {
        for(j=0;j<tn.projection.size();j++){
            cout<<"     "<<"projection: "<<tn.projection[j]<<endl;
        }
    }
    cout<<"has_selection: "<<tn.has_selection<<endl;
    if(tn.has_selection==true) {
        for(j=0;j<tn.selection.size();j++){
            cout<<"     "<<tn.selection[j].aname<<" ";
            cout<<tn.selection[j].op_type<<" ";
            if(tn.selection[j].op_type!=6&&tn.selection[j].op_type!=7) cout<<tn.selection[j].num;
            if(tn.selection[j].op_type==6) cout<<tn.selection[j].str;
            cout<<"  "<<endl;
        }
    }
    cout<<"num_of_recs: "<<tn.num_of_recs<<endl;
    cout<<"tn_attributes: "<<endl;
    for(j=0;j<tn.tn_attributes.size();j++){
        cout<<"     "<<tn.tn_attributes[j].aname<<"  "<<tn.tn_attributes[j].has_restriction<<"  ";
        if(tn.tn_attributes[j].has_restriction==true&&tn.tn_attributes[j].restriction_range.size()!=0) cout<<tn.tn_attributes[j].restriction_range[0]<<" "<<tn.tn_attributes[j].restriction_range[1];
        if(tn.tn_attributes[j].has_restriction==true&&tn.tn_attributes[j].restriction_range.size()==0) cout<<tn.tn_attributes[j].restriction;
        cout<<" "<<endl;
    }
    cout<<"tn_size: "<<tn.tn_size<<endl;
    if(tn.parent==-1) cout<<"no parent:-1"<<endl;
    else{
        cout<<"has parent, parent:"<<tn.parent<<endl;
    }
    if(tn.child[0]==-1) cout<<"no child:-1"<<endl;
    else{
        cout<<"has "<<tn.child.size()<<" child:";
        for(k=0;k<tn.child.size();k++){
            cout<<" "<<tn.child[k];
        }
        cout<<endl;
    }
    cout<<" "<<endl;
}

void print_node_format(Tree query_tree,TreeNode tn, map<string,string>& output_for_etcd, map<string, string>& node2site, string prefix, map<string, string> siteno_to_ip){
    string key_head = "temp_table"+tn.layer+".";
    if(tn.node_type==1){
        output_for_etcd.insert(pair<string, string>(key_head+"type","L")); //fragment node
        string lchild;
        lchild.append("site"+tn.sname.substr(4,1)+"_"+tn.rname);
        output_for_etcd.insert(pair<string, string>(key_head+"children",lchild)); //fragment node        
    } 
    else if(tn.node_type==2){
        output_for_etcd.insert(pair<string, string>(key_head+"type","NL"));
        string table_name1;
        string table_name2;
        for(int i=0;i<query_tree.tn[tn.child[0]].tn_attributes.size();i++){
            if(query_tree.tn[tn.child[0]].tn_attributes[i].aname==tn.join[0]) {
               table_name1="temp_table"+query_tree.tn[tn.child[0]].layer+".";
               table_name2="temp_table"+query_tree.tn[tn.child[1]].layer+".";
            break;
            }
            if(query_tree.tn[tn.child[0]].tn_attributes[i].aname==tn.join[1]) {
               table_name2="temp_table"+query_tree.tn[tn.child[0]].layer+".";
               table_name1="temp_table"+query_tree.tn[tn.child[1]].layer+".";
            break;
            }         
        }
        output_for_etcd.insert(pair<string, string>(key_head+"combine",prefix+table_name1+tn.join[0]+" = "+prefix+table_name2+tn.join[1]));
    }
    else if(tn.node_type==3){ //union node
        output_for_etcd.insert(pair<string, string>(key_head+"type","NL"));
        output_for_etcd.insert(pair<string, string>(key_head+"combine","U"));
    }
    if(tn.has_projection==true) {
        if(tn.node_type==1){  //leave-node
            string proj;
            for(int i=0;i<tn.projection.size();i++){
                proj.append(tn.projection[i].substr(tn.projection[i].find('_')+1,tn.projection[i].length()-1)+",");
            }
            proj.erase(proj.end()-1);//erase last ","
            output_for_etcd.insert(pair<string, string>(key_head+"project",proj));               
        }
        else{  //non-leave node
            string proj;
            for(int i=0;i<tn.projection.size();i++){
                proj.append(tn.projection[i]+",");
            }
            proj.erase(proj.end()-1);//erase last ","
            output_for_etcd.insert(pair<string, string>(key_head+"project",proj));   
        }
    }
    if(tn.has_selection==true){
        string sele;
        for(int i=0;i<tn.selection.size();i++){
            sele.append(tn.selection[i].aname.substr(tn.selection[i].aname.find('_')+1,tn.selection[i].aname.length()-1));
            if(tn.selection[i].op_type==1) sele.append(" >= "+num2str(tn.selection[i].num));
            else if(tn.selection[i].op_type==2) sele.append(" <= "+num2str(tn.selection[i].num));
            else if(tn.selection[i].op_type==3) sele.append(" > "+num2str(tn.selection[i].num));
            else if(tn.selection[i].op_type==4) sele.append(" < "+num2str(tn.selection[i].num));
            else if(tn.selection[i].op_type==5) sele.append(" = "+num2str(tn.selection[i].num));
            else if(tn.selection[i].op_type==6) sele.append(" = "+tn.selection[i].str);
            sele.append(",");
        }
        sele.erase(sele.end()-1); //erase last ","
        output_for_etcd.insert(pair<string, string>(key_head+"select",sele));       
    }
    if(tn.child[0]!=-1){ //if has child
        string chil;
        for(int i=0;i<tn.child.size();i++){
            string child_ip;
            child_ip = siteno_to_ip.find(query_tree.tn[tn.child[i]].sname)->second;
            chil.append(prefix+"temp_table"+query_tree.tn[tn.child[i]].layer+":"+child_ip+"|");
            node2site[prefix+"temp_table"+query_tree.tn[tn.child[i]].layer] = query_tree.tn[tn.child[i]].sname;
        }
        chil.erase(chil.end()-1); //erase last "|"
        output_for_etcd.insert(pair<string, string>(key_head+"children",chil));       
    }    
}

void print_tree_format(Tree query_tree, int root,map<string,string>& output_for_etcd, map<string, string>& node2site, string prefix, map<string, string> siteno_to_ip){
    queue<int> tn_queue;
    tn_queue.push(root);
    while(tn_queue.size()>0){
        int current_addr=tn_queue.front();
        print_node_format(query_tree,query_tree.tn[current_addr],output_for_etcd, node2site, prefix, siteno_to_ip);
        if(query_tree.tn[current_addr].child[0]!=-1){
            for(int i=0;i<query_tree.tn[current_addr].child.size();i++){
                tn_queue.push(query_tree.tn[current_addr].child[i]);
            }
        }
        tn_queue.pop();
    }        
}

void print_tree(Tree query_tree, int root){
    queue<int> tn_queue;
    tn_queue.push(root);
  //  int current_layer=0;
    while(tn_queue.size()>0){
        int current_addr=tn_queue.front();
        /*if(query_tree.tn[current_addr].layer>current_layer){
            current_layer=query_tree.tn[current_addr].layer;
            cout<<"layer"<<current_layer<<" : _______________________________________________________________"<<endl;
        }*/
        cout<<"node num: "<<current_addr<<endl;
        print_node(query_tree.tn[current_addr]);
        if(query_tree.tn[current_addr].child[0]!=-1){
            for(int i=0;i<query_tree.tn[current_addr].child.size();i++){
                tn_queue.push(query_tree.tn[current_addr].child[i]);
            }
        }
        tn_queue.pop();
    }
}//层次遍历输出树

void get_par_fname(vector<string>& par_fname,map<string,int> join_or_not,string fname){
    int i,j,k;
    vector<string> temp;
    vector<string> temp1;
    temp.push_back(fname);
    temp1.push_back(fname.substr(0,3));
    for(auto& x:join_or_not){
        if(x.second==1){
            string join_fname=x.first;
            string fname1=join_fname.substr(0,join_fname.find('_'));
            string fname2=join_fname.substr(join_fname.find('_')+1,join_fname.length()-1);
            if(fname1==fname) {temp.push_back(fname2);temp1.push_back(fname2.substr(0,3));}
            else if(fname2==fname) {temp.push_back(fname1);temp1.push_back(fname1.substr(0,3));}
        }
    }
    if(temp1.size()>1){
        for(auto& x:join_or_not){
            string join_fname=x.first;
            string fname1=join_fname.substr(0,join_fname.find('_'));
            string fname2=join_fname.substr(join_fname.find('_')+1,join_fname.length()-1);
            if(find(temp1.begin(),temp1.end(),fname1.substr(0,3))!=temp1.end()&&find(temp1.begin(),temp1.end(),fname2.substr(0,3))!=temp1.end()){
                join_or_not.erase(join_fname);
            }
        }
        for(i=0;i<temp.size();i++){
            get_par_fname(par_fname,join_or_not,temp[i]);
        }
    }
    par_fname.insert(par_fname.end(),temp.begin(),temp.end());
}

void label_layer(Tree& query_tree_return,int root){
    if(query_tree_return.tn[root].child[0]!=-1){
        for(int i=0;i<query_tree_return.tn[root].child.size();i++){
            query_tree_return.tn[query_tree_return.tn[root].child[i]].layer.append(query_tree_return.tn[root].layer+"_"+num2str(i+1));
            label_layer(query_tree_return,query_tree_return.tn[root].child[i]);
        }
    }
}

void get_query_tree(map<string,string>& output_for_etcd1, map<string, string>& node2site, vector<Relation> relations,SelectStatement sql,string prefix, map<string, string> siteno_to_ip){
    // siteno_to_ip.insert(pair<string, string>("site1","10.77.70.172"));
    // siteno_to_ip.insert(pair<string, string>("site2","10.77.70.188"));
    // siteno_to_ip.insert(pair<string, string>("site3","10.77.70.189"));
    // siteno_to_ip.insert(pair<string, string>("site4","10.77.70.189"));
    //void initialization(vector<Relation>& relations,SelectStatement& sql);
    //initialization(relations,sql);
    //string prefix="query_0_0_";
    //Suppose all these have been assigned value properly
    int i,j,k,ii,jj,kk; //circulation variable
    for(i=0;i<relations.size();i++){
        for(j=0;j<relations[i].frags.size();j++){
            for(k=0;k<relations[i].frags[j].vf_condition.size();k++){
                relations[i].frags[j].vf_condition[k]=relations[i].rname+"_"+relations[i].frags[j].vf_condition[k];
            }
            for(k=0;k<relations[i].frags[j].hf_condition.size();k++){
                relations[i].frags[j].hf_condition[k].aname=relations[i].rname+"_"+relations[i].frags[j].hf_condition[k].aname;
            }
            relations[i].frags[j].fname=relations[i].frags[j].fname.substr(0,3)+relations[i].frags[j].fname.substr(relations[i].frags[j].fname.length()-1,1);
        }
        for(j=0;j<relations[i].attributes.size();j++){
            relations[i].attributes[j].aname=relations[i].rname+"_"+relations[i].attributes[j].aname;
        }
    }

    string query_site = "site" + prefix.substr(6,1); //发出请求的机器的站点号，假设从1开始计数
    //get query_site
    Tree query_tree;
    Tree query_tree_return;
    query_tree.n=0;
    //generate required query_tree

    vector<Relation> pat_relat; //participant_relations
    for(i=0;i<sql.from.size();i++){
        for(j=0;j<relations.size();j++){
            if(sql.from[i]==relations[j].rname){
                pat_relat.push_back(relations[j]);
                break;
            }
        }
    }
    //find all participant relations

    vector<string> pat_attri_name;
    vector<string> nonselection_attri_name;
    pat_attri_name.assign(sql.select.begin(),sql.select.end());
    nonselection_attri_name.assign(sql.select.begin(),sql.select.end());
    for(i=0;i<sql.where.size();i++){
       if(sql.where[i].op_type==7){ //join类
           pat_attri_name.insert(pat_attri_name.end(),sql.where[i].join.begin(),sql.where[i].join.end());
           nonselection_attri_name.insert(nonselection_attri_name.end(),sql.where[i].join.begin(),sql.where[i].join.end());
       }
       else { //其它类
           pat_attri_name.push_back(sql.where[i].aname);
       }
    } 
    //find participant_attribute_name e.g.customer_name book_copies. Notice that repetition may exist

    vector<Attribute> pat_attri;
    for(i=0;i<pat_attri_name.size();i++){
        bool find_pat_attri=false;
        for(j=0;j<pat_relat.size();j++){
            for(k=0;k<pat_relat[j].attributes.size();k++){
                if(pat_attri_name[i]==pat_relat[j].attributes[k].aname){
                    pat_attri.push_back(pat_relat[j].attributes[k]);
                    find_pat_attri=true;
                    break;
                }
            }
            if(find_pat_attri==true) break;
        }
    }
    //find all participant attributes. Notice that repetition may exist

   for(i=0;i<pat_relat.size();i++){
       if(pat_relat[i].is_horizontal==true) continue;
       vector<Attribute> pat_attri_this;
       for(j=0;j<pat_attri.size();j++){
           int index = pat_attri[j].aname.find('_');
           if(pat_attri[j].aname.substr(0,index) == pat_relat[i].rname){
               pat_attri_this.push_back(pat_attri[j]);
           }
       } //find participant attribute for this relation
       bool is_pat_attri_this_all_key=true;
       for(j=0;j<pat_attri_this.size();j++){
           if(pat_attri_this[j].is_key==false) is_pat_attri_this_all_key=false;
       }
       if(is_pat_attri_this_all_key==true){
           //keep only the first frag and erase the others
           for(j=1;j<pat_relat[i].frags.size();j++){
               pat_relat[i].frags.erase(pat_relat[i].frags.begin()+j);
               j--;
           }
       }
       else{
           for(j=0;j<pat_relat[i].frags.size();j++){
               bool pruning_vf=true;
               for(k=0;k<pat_relat[i].frags[j].vf_condition.size();k++){
                   for(ii=0;ii<pat_attri_this.size();ii++){
                       if((pat_relat[i].frags[j].vf_condition[k]==pat_attri_this[ii].aname)&&(pat_attri_this[ii].is_key==false)) pruning_vf=false;
                   }
               }
              if(pruning_vf==true){
                  pat_relat[i].frags.erase(pat_relat[i].frags.begin()+j);
                  j--;
              }
           }
       }
   }
   //Pruning Vertical Fragments according to participant attributes.

   for(i=0;i<pat_relat.size();i++){
       for(j=0;j<pat_relat[i].frags.size();j++){
           if(pat_relat[i].frags[j].is_horizontal==false){
               vector<string> projection1;
               projection1.assign(nonselection_attri_name.begin(),nonselection_attri_name.end());
               if(pat_relat[i].frags.size()>=2){  //imply key must be maintained for later join
                   for(k=0;k<pat_relat[i].attributes.size();k++){
                       if(pat_relat[i].attributes[k].is_key==true){
                           projection1.push_back(pat_relat[i].attributes[k].aname);
                       }
                   }
               }
               vector<Predicate> temp;
               TreeNode temp_tn;
               temp_tn.create_treenode(1,pat_relat[i].frags[j].fname,pat_relat[i].rname,pat_relat[i].frags[j].sname,pat_relat[i].frags[j].vf_condition,sql,projection1,pat_relat[i].attributes,1,temp,pat_relat[i].num_of_recs);
               temp_tn.get_tn_size();
               query_tree.tn.push_back(temp_tn);
               query_tree.n++;
           }
           else{  //horizontal fragments may be pruned
               bool prune_this_frag=false;
               for(k=0;k<pat_relat[i].frags[j].hf_condition.size();k++){
                   for(ii=0;ii<sql.where.size();ii++){
                       if(pat_relat[i].frags[j].hf_condition[k].aname==sql.where[ii].aname){
                           if(exist_conflict(pat_relat[i].frags[j].hf_condition[k],sql.where[ii])==true){
                               prune_this_frag=true;
                               pat_relat[i].frags.erase(pat_relat[i].frags.begin()+j);
                               j--;
                               break;
                           }
                       }
                   }
                   if(prune_this_frag==true) break;
               }
               if(prune_this_frag==false){
                   vector<string> attri_names;
                   for(k=0;k<pat_relat[i].attributes.size();k++){
                       attri_names.push_back(pat_relat[i].attributes[k].aname);
                   }
                   TreeNode temp_tn;
                   temp_tn.create_treenode(1,pat_relat[i].frags[j].fname,pat_relat[i].rname,pat_relat[i].frags[j].sname,attri_names,sql,nonselection_attri_name,pat_relat[i].attributes,2,pat_relat[i].frags[j].hf_condition,pat_relat[i].num_of_recs);
                   temp_tn.get_tn_size();
                   query_tree.tn.push_back(temp_tn);
                   query_tree.n++;
               }
           }
       }
   }
   //generate fragments nodes and do selection, projection

    map<string,int> join_or_not;
   for(i=0;i<sql.where.size();i++){
       if(sql.where[i].op_type==7){
           //bool has_vf=false;
           string rname1, rname2;
           rname1=sql.where[i].join[0].substr(0,sql.where[i].join[0].find('_'));
           rname2=sql.where[i].join[1].substr(0,sql.where[i].join[1].find('_'));
           /*for(j=0;j<pat_relat.size();j++){
               if(pat_relat[j].rname==rname1&&pat_relat[j].is_horizontal==false) {has_vf=true; break;}
               if(pat_relat[j].rname==rname2&&pat_relat[j].is_horizontal==false) {has_vf=true; break;}
           }
           if(has_vf==true) continue;  //vf cannot be pruned
            */
           //vf can also cause pruning, see initialization 10
           map<string, vector<double>> map_fname_restriction1;
           map<string, vector<double>> map_fname_restriction2;
           for(j=0;j<query_tree.tn.size();j++){
               if(query_tree.tn[j].node_type==1&&query_tree.tn[j].fname.substr(0,3)==rname1.substr(0,3)){
                   vector<double> t4;
                   find_rres(query_tree.tn[j].tn_attributes,sql.where[i].join[0],t4);
                   map_fname_restriction1.insert(pair<string, vector<double>>(query_tree.tn[j].fname,t4));
               }
               if(query_tree.tn[j].node_type==1&&query_tree.tn[j].fname.substr(0,3)==rname2.substr(0,3)){
                   vector<double> t4;
                   find_rres(query_tree.tn[j].tn_attributes,sql.where[i].join[1],t4);
                   map_fname_restriction2.insert(pair<string, vector<double>>(query_tree.tn[j].fname,t4));
               }
           }
           for(auto& x:map_fname_restriction1){
               bool prune_this=true;
               for(auto& y:map_fname_restriction2){
                   string join_fname=get_join_name(x.first,y.first);
                   vector<double> intersec;
                   get_intersection(x.second,y.second,intersec);
                   if(intersec[0]>intersec[1]){
                       join_or_not.insert(pair<string, int>(join_fname,0));
                   }
                   else{
                       prune_this=false;
                       join_or_not.insert(pair<string, int>(join_fname,1));
                   }
               }
               if(prune_this==true){
                   for(ii=0;ii<query_tree.tn.size();ii++){
                       if(query_tree.tn[ii].fname==x.first){
                           query_tree.tn.erase(query_tree.tn.begin()+ii);
                           query_tree.n--;
                           break;
                       }
                   }
               }
           }
           for(auto& x:map_fname_restriction2){
               bool prune_this=true;
               for(auto& y:map_fname_restriction1){
                   vector<double> intersec;
                   get_intersection(x.second,y.second,intersec);
                   if(intersec[0]<=intersec[1]){
                       prune_this=false;
                       break;
                   }
               }
               if(prune_this==true){
                   for(ii=0;ii<query_tree.tn.size();ii++){
                       if(query_tree.tn[ii].node_type==1&&query_tree.tn[ii].fname==x.first){
                           query_tree.tn.erase(query_tree.tn.begin()+ii);
                           query_tree.n--;
                           break;
                       }
                   }
               }
           }
       }
   }
   //pruning fragments according to join predicate in where

    for(i=0;i<pat_relat.size();i++){
        if(pat_relat[i].is_horizontal==false&&pat_relat[i].frags.size()>=2){
            TreeNode temp_tn;
            temp_tn.node_type=2;
            for(j=0;j<pat_relat[i].attributes.size();j++){
                if(pat_relat[i].attributes[j].is_key==true){
                    temp_tn.join.push_back(pat_relat[i].attributes[j].aname);
                    temp_tn.join.push_back(pat_relat[i].attributes[j].aname);
                }
            }
            int max_tn_size=0;
            temp_tn.num_of_recs=INF;
            vector<TNAttribute> temp0;
            for(j=0;j<pat_relat[i].frags.size();j++){
                for(k=0;k<query_tree.tn.size();k++){
                    if(pat_relat[i].frags[j].fname == query_tree.tn[k].fname){
                        temp_tn.child.push_back(k);
                        // query_tree.tn[k].parent = &temp_tn;
                        if(query_tree.tn[k].tn_size>max_tn_size){
                            max_tn_size=query_tree.tn[k].tn_size;
                            temp_tn.sname=query_tree.tn[k].sname;
                        }
                        if(query_tree.tn[k].num_of_recs<temp_tn.num_of_recs){
                            temp_tn.num_of_recs=query_tree.tn[k].num_of_recs;
                        }//suppose num_of_recs after join equals to vf's minimal num_of_recs;
                        temp0.insert(temp0.end(),query_tree.tn[k].tn_attributes.begin(),query_tree.tn[k].tn_attributes.end());
                        break;
                    }
                }
            }
            temp_tn.has_selection=false;
            vector<TNAttribute> temp1;
            vector<string> tattri_name;
            for(j=0;j<temp0.size();j++) tattri_name.push_back(temp0[j].aname);
            set<string> s(tattri_name.begin(),tattri_name.end());
            tattri_name.assign(s.begin(), s.end());
            for(j=0;j<tattri_name.size();j++){
                for(k=0;k<temp0.size();k++){
                    if(tattri_name[j]==temp0[k].aname){
                        temp1.push_back(temp0[k]);
                        break;
                    }
                }
            }
            //then remove repetition in temp0 as temp1
            temp_tn.has_projection=false;
            for(j=0;j<temp1.size();j++){
                if(find(nonselection_attri_name.begin(),nonselection_attri_name.end(),temp1[j].aname)==nonselection_attri_name.end()){
                    temp_tn.has_projection=true;
                }
                else{
                    temp_tn.tn_attributes.push_back(temp1[j]);
                    temp_tn.projection.push_back(temp1[j].aname);
                }
            }
            temp_tn.eliminate_attribute();
            temp_tn.get_tn_size();
            temp_tn.parent=-1;
            query_tree.tn.push_back(temp_tn);
            for(j=0;j<query_tree.tn[query_tree.n].child.size();j++){
                query_tree.tn[query_tree.tn[query_tree.n].child[j]].parent=query_tree.n;
            }
            query_tree.n++;
        }
    }
    //join vertical fragments

   int max_r_size=0;
   string max_r;
   bool is_joined_vf=false;
   vector<string> process_sites;
   string ps;
#if  use_transport_optimization
   for(i=0;i<query_tree.n;i++){
       if(query_tree.tn[i].parent==-1){
           if(query_tree.tn[i].node_type==2&&query_tree.tn[i].tn_size>max_r_size){
               max_r_size=query_tree.tn[i].tn_size;
               is_joined_vf=true;
               max_r=query_tree.tn[query_tree.tn[i].child[0]].fname.substr(0,3);
               ps=query_tree.tn[i].sname;
           }
           else{
               int t5=0;
               for(j=0;j<query_tree.n;j++){
                   if(query_tree.tn[i].fname.substr(0,3)==query_tree.tn[j].fname.substr(0,3)){
                       t5+=query_tree.tn[j].tn_size;
                   }
               }
               if(t5>max_r_size){
                   max_r_size=t5;
                   max_r=query_tree.tn[i].fname.substr(0,3);
                   is_joined_vf=false;
               }
           }
       }
   } 
   //find Rp
   int least_difference=INF;
   string best_ps;
   if(is_joined_vf==true){
       process_sites.push_back(ps);
   }
   else{
       for(i=0;i<query_tree.tn.size();i++){
           if(query_tree.tn[i].fname.substr(0,3)==max_r){  //for each fragments in max_r, determine whether it is a processing site
               int ex_as_ps=0; //expense as processing site
               int ex_as_nps=query_tree.tn[i].tn_size;
               for(j=0;j<query_tree.tn.size();j++){
                   if(query_tree.tn[j].parent==-1&&query_tree.tn[j].sname!=query_tree.tn[i].sname&&query_tree.tn[j].fname.substr(0,3)!=query_tree.tn[i].fname.substr(0,3)){
                       string jf=get_join_name(query_tree.tn[j].fname,query_tree.tn[i].fname);
                       if(join_or_not.find(jf)->second==1){
                           ex_as_ps+=query_tree.tn[j].tn_size;
                       }
                   }
               }
               if(ex_as_ps<=ex_as_nps){
                   process_sites.push_back(query_tree.tn[i].sname);
               }
               else{
                   if((ex_as_ps-ex_as_nps)<least_difference){
                       least_difference=ex_as_ps-ex_as_nps;
                       best_ps=query_tree.tn[i].sname;
                   }
               }
           }
       }
   }
   if(process_sites.size()==0){
       process_sites.push_back(best_ps);
   }
   //determine k: is this site a processing site?
#else
    for(i=0;i<query_tree.tn.size();i++){
        if(query_tree.tn[i].node_type==1){ //chose the first fragment as Rp and processing site
            if(query_tree.tn[i].parent!=-1){ 
                is_joined_vf=true;
                max_r = query_tree.tn[i].fname.substr(0,3);
                process_sites.push_back(query_tree.tn[query_tree.tn[i].parent].sname);
            }
            else{
                is_joined_vf=false;
                max_r = query_tree.tn[i].fname.substr(0,3);
                process_sites.push_back(query_tree.tn[i].sname);
            }
            break;
        }
    }
#endif

   for(i=0;i<sql.where.size();i++){
       if(sql.where[i].op_type==7){
           string rname1, rname2;
           rname1=sql.where[i].join[0].substr(0,sql.where[i].join[0].find('_'));
           rname2=sql.where[i].join[1].substr(0,sql.where[i].join[1].find('_'));
           for(j=0;j<pat_relat.size();j++){
               if(((pat_relat[j].rname==rname1)||(pat_relat[j].rname==rname2))&&(pat_relat[j].is_horizontal==false)){
                   for(k=0;k<query_tree.tn.size();k++){
                       if(query_tree.tn[k].fname.substr(0,3)==rname1.substr(0,3)){
                          for(ii=0;ii<query_tree.tn.size();ii++){
                              if(query_tree.tn[ii].fname.substr(0,3)==rname2.substr(0,3)){
                                  join_or_not.insert(pair<string, int>(get_join_name(query_tree.tn[k].fname,query_tree.tn[ii].fname),1));
                              }
                          }
                       }
                   }
                   break;
               }
           }
       }
   }
   //generate join_or_not for vf involved join
   vector<string> fragments_involved;
   for(i=0;i<query_tree.tn.size();i++){
       if(query_tree.tn[i].node_type==1) fragments_involved.push_back(query_tree.tn[i].fname);
   }
   for(auto& x:join_or_not){
       string join_fname=x.first;
       string fname1=join_fname.substr(0,join_fname.find('_'));
       string fname2=join_fname.substr(join_fname.find('_')+1,join_fname.length()-1);
       if(find(fragments_involved.begin(),fragments_involved.end(),fname1)==fragments_involved.end()||find(fragments_involved.begin(),fragments_involved.end(),fname2)==fragments_involved.end()){
           join_or_not.erase(join_fname);
       }
   }
   //notice that join_or_not may have more pairs than needed, erase not needed pairs
   /*
   cout<<"____________________info begin___________________________"<<endl;
   cout<<"max_relation: "<<max_r<<endl;
   for(i=0;i<process_sites.size();i++){
       cout<<"process_sites "<<i<<": "<<process_sites[i]<<endl;
   }
   for(auto& x:join_or_not){
       cout<<x.first<<" : "<<x.second<<endl;
   }
   cout<<"____________________info end___________________________"<<endl;
   */
   TreeNode temp_tn;
   temp_tn.node_type=3;
   temp_tn.has_selection=false;
   temp_tn.has_projection=false;
   temp_tn.sname=process_sites[0];
   temp_tn.num_of_recs=0;
   temp_tn.parent=-1;
   bool do_union=false;
   if(is_joined_vf==false){
       for(i=0;i<query_tree.tn.size();i++){
           if(query_tree.tn[i].fname.substr(0,3)==max_r&&find(process_sites.begin(),process_sites.end(),query_tree.tn[i].sname)==process_sites.end()){
               if(do_union==false){
                   do_union=true;
                   temp_tn.tn_attributes.assign(query_tree.tn[i].tn_attributes.begin(),query_tree.tn[i].tn_attributes.end());
               }  //note that restriction range in tn_attributes no longer matters
               temp_tn.num_of_recs+=query_tree.tn[i].num_of_recs;
               temp_tn.child.push_back(i);
               //query_tree.tn[i].parent=&temp_tn;
           }
       }
   }
   if(do_union==true){
       for(i=0;i<query_tree.tn.size();i++){
           if(query_tree.tn[i].fname.substr(0,3)==max_r&&query_tree.tn[i].sname==process_sites[0]){
               temp_tn.num_of_recs+=query_tree.tn[i].num_of_recs;
               temp_tn.child.push_back(i);
               //query_tree.tn[i].parent=&temp_tn;
           }
       }
       temp_tn.get_tn_size();
       query_tree.tn.push_back(temp_tn);
       for(i=0;i<query_tree.tn[query_tree.n].child.size();i++){
           query_tree.tn[query_tree.tn[query_tree.n].child[i]].parent=query_tree.n;
       }
       query_tree.n++;
   }
   //union all nps max_r fragments to ps[0]
   /*for(i=0;i<query_tree.tn.size();i++){
       cout<<"node num: "<<i<<endl;
       print_node(query_tree.tn[i]);
   }*/

   vector<Tree> subtree;
   for(i=0;i<process_sites.size();i++){
       Tree temp_subtree;
       temp_subtree.n=0;
       //for every process sites, generate a subtree;
       vector<string> par_fname;//the fname for all participant fragments in this site
       for(j=0;j<query_tree.tn.size();j++){
           if((query_tree.tn[j].fname.substr(0,3)==max_r)&&(query_tree.tn[j].sname==process_sites[i])){
               if(query_tree.tn[j].parent==-1){  //one vf or one hf
                   get_par_fname(par_fname,join_or_not,query_tree.tn[j].fname);
               }
               else{ //joined vf or unioned hf
                   for(k=0;k<query_tree.tn[query_tree.tn[j].parent].child.size();k++){
                       int child_addr = query_tree.tn[query_tree.tn[j].parent].child[k];
                       get_par_fname(par_fname,join_or_not,query_tree.tn[child_addr].fname);
                   }
               }
           }
       }
       set<string> s(par_fname.begin(),par_fname.end());
       par_fname.assign(s.begin(),s.end());
       //remove repetition in par_fname
       /*cout<<par_fname.size()<<endl;
       for(j=0;j<par_fname.size();j++){
           cout<<par_fname[j]<<endl;
       }*/
       vector<string> apb;
       for(j=0;j<par_fname.size();j++){
           if(find(apb.begin(),apb.end(),par_fname[j])==apb.end()){
               for(k=0;k<query_tree.tn.size();k++){
                   if(query_tree.tn[k].node_type==1&&par_fname[j]==query_tree.tn[k].fname){
                       if(query_tree.tn[k].parent==-1){ //one vf or one hf
                           //cout<<"SINGLE:"<<query_tree.tn[k].fname<<endl;
                           temp_subtree.tn.push_back(query_tree.tn[k]);
                           temp_subtree.n++;
                           apb.push_back(query_tree.tn[k].fname);
                       }
                       else{ //joined vf or unioned hf
                           //cout<<"Multiple:"<<query_tree.tn[k].fname<<endl;
                           int p_addr;
                           p_addr=temp_subtree.n;
                           vector<int> c_addr;
                           for(ii=0;ii<query_tree.tn[query_tree.tn[k].parent].child.size();ii++){
                              c_addr.push_back(query_tree.tn[query_tree.tn[k].parent].child[ii]);
                           }
                           temp_subtree.tn.push_back(query_tree.tn[query_tree.tn[k].parent]);
                           temp_subtree.tn[temp_subtree.n].child.clear();
                           temp_subtree.n++;
                           for(ii=0;ii<c_addr.size();ii++){
                               int child_addr=c_addr[ii];
                               temp_subtree.tn.push_back(query_tree.tn[child_addr]);
                               apb.push_back(query_tree.tn[child_addr].fname);
                               temp_subtree.tn[temp_subtree.n].parent=p_addr;
                               temp_subtree.tn[p_addr].child.push_back(temp_subtree.n);
                               temp_subtree.n++;
                           }
                       }
                       break;
                   }
               }
           }
       }
       //for every fname in par_fname, generate a subtreenode for one vf and one hf or a serials of nodes for joined vf and unioned hf
       for(j=0;j<temp_subtree.tn.size();j++){
           if(temp_subtree.tn[j].parent==-1&&temp_subtree.tn[j].node_type==1){  //one vf or one hf
               bool do_union=false;
               for(k=0;k<temp_subtree.tn.size();k++){
                   if(temp_subtree.tn[k].fname.substr(0,3)==temp_subtree.tn[j].fname.substr(0,3)&&k!=j){
                       do_union=true;
                       break;
                   }
               }
               //cout<<temp_subtree.tn[j].fname<<" : "<<do_union<<endl;
               if(do_union==true){
                   TreeNode temp_subtreenode;
                   temp_subtreenode.node_type=3;
                   temp_subtreenode.has_selection=false;
                   temp_subtreenode.has_projection=false;
                   temp_subtreenode.sname=process_sites[i];
                   temp_subtreenode.tn_attributes.assign(temp_subtree.tn[j].tn_attributes.begin(),temp_subtree.tn[j].tn_attributes.end());
                   //attributes restriction range no longer matters
                   temp_subtreenode.num_of_recs=temp_subtree.tn[j].num_of_recs;
                   temp_subtreenode.child.push_back(j);
                   //temp_subtree.tn[j].parent=&temp_subtreenode;
                   for(k=0;k<temp_subtree.tn.size();k++){
                       if(temp_subtree.tn[k].fname.substr(0,3)==temp_subtree.tn[j].fname.substr(0,3)&&k!=j){
                           temp_subtreenode.child.push_back(k);
                           //temp_subtree.tn[k].parent=&temp_subtreenode;
                           temp_subtreenode.num_of_recs+=temp_subtree.tn[k].num_of_recs;
                       }
                   }
                   temp_subtreenode.get_tn_size();
                   temp_subtreenode.parent= -1;
                   temp_subtree.tn.push_back(temp_subtreenode);
                   for(k=0;k<temp_subtree.tn[temp_subtree.n].child.size();k++){
                       int child_addr = temp_subtree.tn[temp_subtree.n].child[k];
                       temp_subtree.tn[child_addr].parent = temp_subtree.n;
                   }
                   temp_subtree.n++;
               }
           }
       }
       //union all one hf belongs to the same relation

       for(j=0;j<sql.where.size();j++){
           if(sql.where[j].op_type==7){
               string rname1=sql.where[j].join[0].substr(0,3);
               string rname2=sql.where[j].join[1].substr(0,3);
               int addr1;
               int addr2;
               for(k=0;k<temp_subtree.tn.size();k++){
                   if(temp_subtree.tn[k].fname.substr(0,3)==rname1){
                       addr1 = k;
                       while(temp_subtree.tn[addr1].parent!=-1){
                           addr1 = temp_subtree.tn[addr1].parent;
                       }
                       break;
                   }
               }
               for(k=0;k<temp_subtree.tn.size();k++){
                   if(temp_subtree.tn[k].fname.substr(0,3)==rname2){
                       addr2 = k;
                       while(temp_subtree.tn[addr2].parent!=-1){
                           addr2 = temp_subtree.tn[addr2].parent;
                       }
                       break;
                   }
               }
               TreeNode temp_join_node;
               temp_join_node.node_type=2;
               temp_join_node.join.assign(sql.where[j].join.begin(),sql.where[j].join.end());
               temp_join_node.has_selection=false;
               temp_join_node.has_projection=false;
               temp_join_node.num_of_recs=min(temp_subtree.tn[addr1].num_of_recs,temp_subtree.tn[addr2].num_of_recs);
               //assume that after join, num_of_recs equals the smaller part
               temp_join_node.tn_attributes.insert(temp_join_node.tn_attributes.end(),temp_subtree.tn[addr1].tn_attributes.begin(),temp_subtree.tn[addr1].tn_attributes.end());
               temp_join_node.tn_attributes.insert(temp_join_node.tn_attributes.end(),temp_subtree.tn[addr2].tn_attributes.begin(),temp_subtree.tn[addr2].tn_attributes.end());
               //Note that join attribute exist duplication
               temp_join_node.get_tn_size();
               temp_join_node.sname=process_sites[i];
               temp_join_node.parent=-1;
               temp_join_node.child.push_back(addr1);
               temp_join_node.child.push_back(addr2);
               temp_subtree.tn[addr1].parent=temp_subtree.n;
               temp_subtree.tn[addr2].parent=temp_subtree.n;
               temp_subtree.tn.push_back(temp_join_node);
               temp_subtree.n++;
           }
       }
       //join different relations, can be one vf,one hf,joined vf,unioned hf
       int rt;
       for(j=0;j<temp_subtree.tn.size();j++){
           if(temp_subtree.tn[j].parent==-1) {rt=j;break;}
       }
       temp_subtree.tn[rt].determine_projection(sql.select);
       //projection on final output attributes
       temp_subtree.tn[rt].eliminate_attribute();
       temp_subtree.tn[rt].get_tn_size();
       subtree.push_back(temp_subtree);
   }
   
   if(subtree.size()==1){
       query_tree_return=subtree[0];
   }
   else{ //do union
       TreeNode temp_union_node;
       temp_union_node.node_type=3;
       temp_union_node.has_selection=false;
       temp_union_node.has_projection=false;
       temp_union_node.tn_attributes.assign(subtree[0].tn[subtree[0].n-1].tn_attributes.begin(),subtree[0].tn[subtree[0].n-1].tn_attributes.end());
       temp_union_node.num_of_recs=0;
       for(i=0;i<subtree.size();i++){
           temp_union_node.num_of_recs+=subtree[i].tn[subtree[i].n-1].num_of_recs;
       }
       temp_union_node.sname = query_site;
       temp_union_node.get_tn_size();
       temp_union_node.parent=-1;
       query_tree_return.n=0;
       for(i=0;i<subtree.size();i++){
           query_tree_return.tn.insert(query_tree_return.tn.end(),subtree[i].tn.begin(),subtree[i].tn.end());
           for(j=query_tree_return.n;j<query_tree_return.n+subtree[i].n;j++){
               if(query_tree_return.tn[j].parent!=-1){
                   query_tree_return.tn[j].parent+=query_tree_return.n;
               }
               for(k=0;k<query_tree_return.tn[j].child.size();k++){
                   if(query_tree_return.tn[j].child[k]!=-1){
                       query_tree_return.tn[j].child[k]+=query_tree_return.n;
                   }
               }
           }
           query_tree_return.n+=subtree[i].n;
           temp_union_node.child.push_back(query_tree_return.n-1);
       }
       query_tree_return.tn.push_back(temp_union_node);
       query_tree_return.n++;
       for(i=0;i<query_tree_return.tn[query_tree_return.n-1].child.size();i++){
           int chld_addr = query_tree_return.tn[query_tree_return.n-1].child[i];
           query_tree_return.tn[chld_addr].parent = query_tree_return.n-1;
       }
   }
   //union all subtree as a complete query tree:query_tree_return
   
   /*for(i=0;i<query_tree_return.tn.size();i++){
       cout<<"node num: "<<i<<endl;
       print_node(query_tree_return.tn[i]);
   }*/
   int root;
   for(i=0;i<query_tree_return.tn.size();i++) {
       if(query_tree_return.tn[i].parent==-1) {root=i;break;}
   }
   query_tree_return.tn[root].sname=query_site;
   label_layer(query_tree_return,root);   //label layer for each tree node
/*
   cout<<"__________________________output format 1 begin__________________________"<<endl;
   print_tree(query_tree_return, root);
   cout<<"__________________________output format 1 end__________________________"<<endl;
*/
    map<string,string> output_for_etcd;
    print_tree_format(query_tree_return,root,output_for_etcd, node2site, prefix, siteno_to_ip);
    for(auto& x:output_for_etcd){
        output_for_etcd1.insert(pair<string, string>(prefix+x.first,x.second)); //fragment node
    }
    /*for(auto& x:output_for_etcd1){
        cout<<x.first<<"--------->"<<x.second<<endl;
    }*/
    return;
}
